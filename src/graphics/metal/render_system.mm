#include "graphics/render_system.h"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>

#include "core/exception.h"
#include "core/macos/macos_ios_utility.h"
#include "core/matrix4.h"
#include "core/vector3.h"
#include "core/window.h"
#include "graphics/buffer.h"
#include "graphics/pipeline.h"
#include "graphics/render_entity.h"
#include "graphics/render_target.h"
#include "log/log.h"

namespace
{

struct DefaultUniform
{
    iris::Matrix4 projection;
    iris::Matrix4 view;
    iris::Matrix4 model;
    iris::Matrix4 normal_matrix;
    iris::Matrix4 bones[100];
    float time;
};

struct LightUniform
{
    float direction[4];
    iris::Matrix4 proj;
    iris::Matrix4 view;
};

/**
 * Helper function to set uniforms for a render pass.
 *
 * @param render_encoder
 *   Render encoder for current pass.
 *
 * @param camera
 *   Camera for current render pass.
 *
 * @param entity
 *   Entity being rendered.
 *
 * @param lights
 *   Lights effecting entity.
 */
void set_uniforms(
    id<MTLRenderCommandEncoder> render_encoder,
    const iris::Camera &camera,
    iris::RenderEntity *entity,
    const std::vector<iris::Light *> lights)
{
    static const iris::Matrix4 metal_translate{
        {{1.0f ,0.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f, 0.0f,
          0.0f, 0.0f, 0.5f, 0.5f,
          0.0f, 0.0f, 0.0f, 1.0f}}
    };

    // copy uniform data into a struct
    DefaultUniform uniform_data{
        iris::Matrix4::transpose(metal_translate * camera.projection()),
        iris::Matrix4::transpose(camera.view()),
        iris::Matrix4::transpose(entity->transform()),
        iris::Matrix4::transpose(entity->normal_transform())
    };

    auto iter = std::begin(uniform_data.bones);
    for(const auto &bone : entity->skeleton().transforms())
    {
        *iter = iris::Matrix4::transpose(bone);
        ++iter;
    }

    static const auto start = std::chrono::steady_clock::now();
    const auto now = std::chrono::steady_clock::now() - start;
    uniform_data.time = static_cast<float>(
        std::chrono::duration_cast<std::chrono::milliseconds>(now).count());

    std::vector<LightUniform> light_data(lights.size());

    for (auto i = 0u; i < lights.size(); ++i)
    {
        const auto direction = lights[i]->direction();
        std::memcpy(
            &light_data[i].direction,
            reinterpret_cast<const std::uint8_t*>(&direction),
            sizeof(direction));
        light_data[i].proj = iris::Matrix4::transpose(metal_translate * lights[i]->shadow_camera().projection());
        light_data[i].view = iris::Matrix4::transpose(lights[i]->shadow_camera().view());
        
    }

    [render_encoder setVertexBytes:static_cast<const void*>(&uniform_data) length:sizeof(uniform_data) atIndex:1];
    [render_encoder setVertexBytes:static_cast<const void*>(light_data.data()) length:light_data.size() * sizeof(LightUniform) atIndex:2];
    [render_encoder setFragmentBytes:static_cast<const void*>(light_data.data()) length:light_data.size() * sizeof(LightUniform) atIndex:0];
}

}

namespace iris
{

/**
 * Struct containing implementation specific data.
 */
struct RenderSystem::implementation
{
    implementation(
        CAMetalLayer *layer,
        id<MTLCommandQueue> command_queue,
        MTLRenderPassDescriptor *descriptor,
        id<MTLDepthStencilState> depth_stencil_state)
    : layer(layer),
      command_queue(command_queue),
      descriptor(descriptor),
      depth_stencil_state(depth_stencil_state)
    { }

    CAMetalLayer *layer;

    id<MTLCommandQueue> command_queue;
    
    MTLRenderPassDescriptor *descriptor;

    id<MTLDepthStencilState> depth_stencil_state;
};

RenderSystem::RenderSystem(float width, float height, RenderTarget *screen_target)
    : screen_target_(screen_target)
    , impl_(nullptr)
{
    // get metal device handle
    const auto *device = iris::core::utility::metal_device();

    const auto command_queue = [device newCommandQueue];
    if(command_queue == nullptr)
    {
        throw iris::Exception("could not creare command queue");
    }

    const auto scale = Window::screen_scale();;

    // create and setup descriptor for depth texture
    auto *texture_description =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                           width:width * scale
                                                          height:height * scale
                                                       mipmapped:NO];
    [texture_description setResourceOptions:MTLResourceStorageModePrivate];
    [texture_description setUsage:MTLTextureUsageRenderTarget];

    // create descriptor for depth checking
    auto *depth_stencil_descriptor = [MTLDepthStencilDescriptor new];
    [depth_stencil_descriptor setDepthCompareFunction:MTLCompareFunctionLess];
    [depth_stencil_descriptor setDepthWriteEnabled:YES];

    // create depth state
    auto *depth_stencil_state = [device newDepthStencilStateWithDescriptor:depth_stencil_descriptor];

    // create and setup a render pass descriptor
    auto *render_pass_descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setLoadAction:MTLLoadActionClear];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setClearColor:MTLClearColorMake(0.77f,0.83f,0.9f,1.0f)];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setStoreAction:MTLStoreActionStore];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setTexture:nullptr];
    [[render_pass_descriptor depthAttachment] setTexture:nullptr];
    [[render_pass_descriptor depthAttachment] setClearDepth:1.0f];
    [[render_pass_descriptor depthAttachment] setLoadAction:MTLLoadActionClear];
    [[render_pass_descriptor depthAttachment] setStoreAction:MTLStoreActionStore];
    
    // create implementation struct
    impl_ = std::make_unique<implementation>(
        core::utility::metal_layer(),
        command_queue,
        render_pass_descriptor,
        depth_stencil_state);
}

/** Default */
RenderSystem::~RenderSystem() = default;
RenderSystem::RenderSystem(RenderSystem&&) = default;
RenderSystem& RenderSystem::operator=(RenderSystem&&) = default;

void RenderSystem::render(const Pipeline &pipeline)
{
    auto *device = iris::core::utility::metal_device();
    auto format = MTLPixelFormatRGBA8Unorm;

    auto *drawable = [impl_->layer nextDrawable];
    auto *command_buffer = [impl_->command_queue commandBuffer];

    for (const auto &stage : pipeline.stages())
    {
        auto &camera = stage->camera();
        auto *target = stage->target();

        // no target means use screen target
        if (target == nullptr)
        {
            target = screen_target_;
        }

        const auto texture_handle = std::any_cast<id<MTLTexture>>(target->colour_texture()->native_handle());
        impl_->descriptor.colorAttachments[0].texture = texture_handle;
        format = texture_handle.pixelFormat;

        const auto depth_handle = std::any_cast<id<MTLTexture>>(target->depth_texture()->native_handle());
        impl_->descriptor.depthAttachment.texture = depth_handle;

        MTLViewport view_port{
            0.0,
            0.0,
            static_cast<double>(target->colour_texture()->width()),
            static_cast<double>(target->colour_texture()->height()),
            0.0,
            1.0 };

        const auto render_encoder = [command_buffer renderCommandEncoderWithDescriptor:impl_->descriptor];
        [render_encoder setDepthStencilState:impl_->depth_stencil_state];
        [render_encoder setFrontFacingWinding:MTLWindingCounterClockwise];
        [render_encoder setCullMode:MTLCullModeBack];
        [render_encoder setViewport: view_port];

        for(const auto &[entity, material, lights] : stage->render_items())
        {
            auto *pipeline_state = std::any_cast<id<MTLRenderPipelineState>>(material->native_handle());

            [render_encoder setTriangleFillMode:MTLTriangleFillModeFill];

            // set wireframe if needed
            if(entity->should_render_wireframe())
            {
                [render_encoder setTriangleFillMode:MTLTriangleFillModeLines];
            }

            for(const auto &mesh : entity->meshes())
            {
                // get vertex Buffer handle
                const auto &vertex_buffer = mesh.vertex_buffer();
                const auto vertex_buffer_native = std::any_cast<id<MTLBuffer>>(vertex_buffer.native_handle());

                // get index Buffer handle
                const auto &index_buffer = mesh.index_buffer();
                const auto index_buffer_native = std::any_cast<id<MTLBuffer>>(index_buffer.native_handle());

                set_uniforms(render_encoder, camera, entity, lights);

                // encode render commands
                [render_encoder setRenderPipelineState:pipeline_state];
                [render_encoder setVertexBuffer:vertex_buffer_native offset:0 atIndex:0];
                [render_encoder setCullMode:MTLCullModeNone];

                const auto textures = material->textures();
                for(const auto *texture : textures)
                {
                    const auto tex_handle = std::any_cast<id<MTLTexture>>(
                        texture->native_handle());
                    [render_encoder setVertexTexture:tex_handle atIndex:texture->texture_id()];
                    [render_encoder setFragmentTexture:tex_handle atIndex:texture->texture_id()];
                }

                const auto type = entity->primitive_type() == PrimitiveType::TRIANGLES
                    ? MTLPrimitiveTypeTriangle
                    : MTLPrimitiveTypeLine;

                // draw command
                [render_encoder
                    drawIndexedPrimitives:type
                    indexCount:index_buffer.element_count()
                    indexType:MTLIndexTypeUInt32
                    indexBuffer:index_buffer_native
                    indexBufferOffset:0];
            }
        }

        [render_encoder endEncoding];

    }

    // final step is to blit the default screen targte to the device
    // note that we don't use the blit function directly, rather we use the
    // metal performance shader image converter as this automatically handles the
    // case where the screen buffer and device buffer have different formats
    auto conversionInfo = CGColorConversionInfoCreate(CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB), CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB));
    auto convert_function = [[MPSImageConversion alloc] initWithDevice:device
                        srcAlpha:MPSAlphaTypeAlphaIsOne
                        destAlpha:MPSAlphaTypeAlphaIsOne
                        backgroundColor:nullptr
                        conversionInfo:conversionInfo];
    
    const auto screen_texture_handle = std::any_cast<id<MTLTexture>>(screen_target_->colour_texture()->native_handle());
    [convert_function encodeToCommandBuffer:command_buffer sourceTexture:screen_texture_handle destinationTexture: drawable.texture];

    [command_buffer presentDrawable:drawable];
    [command_buffer commit];
}

}
