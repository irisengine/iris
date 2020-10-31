#include "graphics/render_system.h"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>

#include "core/exception.h"
#include "core/matrix4.h"
#include "core/vector3.h"
#include "graphics/buffer.h"
#include "graphics/render_entity.h"
#include "graphics/sprite.h"
#include "log/log.h"
#include "platform/macos/macos_ios_utility.h"

namespace
{

struct uniform
{
    iris::Matrix4 projection;
    iris::Matrix4 view;
    iris::Matrix4 model;
    iris::Matrix4 normal_matrix;
    iris::Matrix4 bones[100];
};

}

namespace iris
{

/**
 * Struct containing implementation specific data.
 */
struct RenderSystem::implementation
{
    /** Simple constructor which takes a value for each member. */
    implementation(
        id<MTLCommandQueue> command_queue,
        CAMetalLayer *layer,
        MTLRenderPassDescriptor *descriptor,
        id<MTLTexture> depth_texture,
        id<MTLDepthStencilState> depth_stencil_state)
    : command_queue(command_queue),
      layer(layer),
      descriptor(descriptor),
      depth_texture(depth_texture),
      depth_stencil_state(depth_stencil_state)
    { }

    /** Command queue for rendering a frame. */
    id<MTLCommandQueue> command_queue;

    /** Pointer to metal layer to render to. */
    CAMetalLayer *layer;
    
    /** Pointer to render descriptor. */
    MTLRenderPassDescriptor *descriptor;

    /** Texture for depth buffer. */
    id<MTLTexture> depth_texture;

    /** Metal state for depth buffer. */
    id<MTLDepthStencilState> depth_stencil_state;
};

RenderSystem::RenderSystem(float width, float height)
    : scene_(),
      persective_camera_(CameraType::PERSPECTIVE, width, height),
      orthographic_camera_(CameraType::ORTHOGRAPHIC, width, height),
      impl_(nullptr)
{
    // get metal device handle
    const auto *device = platform::utility::metal_device();
    
    // create a new command queue for rendering
    const auto command_queue = [device newCommandQueue];
    if(command_queue == nullptr)
    {
        throw Exception("could not creare command queue");
    }

    const auto scale = platform::utility::screen_scale();

    // create and setup descriptor for depth texture
    auto *texture_description =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                           width:width * scale
                                                          height:height * scale
                                                       mipmapped:NO];
    [texture_description setResourceOptions:MTLResourceStorageModePrivate];
    [texture_description setUsage:MTLTextureUsageRenderTarget];

    // create depth texture
    auto *depth_texture = [device newTextureWithDescriptor:texture_description];

    // create descriptor for depth checking
    auto *depth_stencil_descriptor = [MTLDepthStencilDescriptor new];
    [depth_stencil_descriptor setDepthCompareFunction:MTLCompareFunctionLess];
    [depth_stencil_descriptor setDepthWriteEnabled:YES];

    // create depth state
    auto *depth_stencil_state = [device newDepthStencilStateWithDescriptor:depth_stencil_descriptor];

    // create and setup a render pass descriptor
    auto *render_pass_descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setLoadAction:MTLLoadActionClear];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setClearColor:MTLClearColorMake(0.1f,0.1f,0.1f,0.1f)];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setStoreAction:MTLStoreActionStore];
    [[render_pass_descriptor depthAttachment] setTexture:depth_texture];
    [[render_pass_descriptor depthAttachment] setClearDepth:1.0f];
    [[render_pass_descriptor depthAttachment] setLoadAction:MTLLoadActionClear];
    [[render_pass_descriptor depthAttachment] setStoreAction:MTLStoreActionDontCare];
    
    // create implementation struct
    impl_ = std::make_unique<implementation>(
        command_queue,
        platform::utility::metal_layer(),
        render_pass_descriptor,
        depth_texture,
        depth_stencil_state);
}

/** Default */
RenderSystem::~RenderSystem() = default;
RenderSystem::RenderSystem(RenderSystem&&) = default;
RenderSystem& RenderSystem::operator=(RenderSystem&&) = default;

void RenderSystem::render()
{
    // using an autoreleasepool allows us to release the drawable object as soon
    // as we are done with it and prevents Core Animation from running out of
    // drawables to vend
    @autoreleasepool
    {
        auto *device = iris::platform::utility::metal_device();

        // update descriptor to render to next frame
        const auto drawable = [impl_->layer nextDrawable];
        [[[impl_->descriptor colorAttachments] objectAtIndexedSubscript:0] setTexture:drawable.texture];
      
        const auto *command_buffer = [impl_->command_queue commandBuffer];

        const auto render_encoder = [command_buffer renderCommandEncoderWithDescriptor:impl_->descriptor];
        [render_encoder setDepthStencilState:impl_->depth_stencil_state];
        [render_encoder setFrontFacingWinding:MTLWindingCounterClockwise];
        [render_encoder setCullMode:MTLCullModeBack];

        // render scene
        for(const auto &entity : scene_)
        {
            for(const auto &mesh : entity->meshes())
            {
                const auto vertex_descriptor = std::any_cast<MTLVertexDescriptor*>(mesh.buffer_descriptor().native_handle());

                const auto [vertex_program, fragment_program] =
                    std::any_cast<std::tuple<id<MTLFunction>, id<MTLFunction>>>(entity->material().native_handle());

                // get pipeline state handle
                auto *pipeline_state_descriptor = [[MTLRenderPipelineDescriptor alloc] init];
                [pipeline_state_descriptor setVertexFunction:vertex_program];
                [pipeline_state_descriptor setFragmentFunction:fragment_program];
                pipeline_state_descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
                [pipeline_state_descriptor setDepthAttachmentPixelFormat:MTLPixelFormatDepth32Float];
                [pipeline_state_descriptor setVertexDescriptor:vertex_descriptor];

                auto *pipeline_state =
                    [device newRenderPipelineStateWithDescriptor:pipeline_state_descriptor error:nullptr];

                [render_encoder setTriangleFillMode:MTLTriangleFillModeFill];

                // set wireframe if needed
                if(entity->should_render_wireframe())
                {
                    [render_encoder setTriangleFillMode:MTLTriangleFillModeLines];
                }

                // get vertex Buffer handle
                const auto &vertex_buffer = mesh.buffer_descriptor().vertex_buffer();
                const auto vertex_buffer_native = std::any_cast<id<MTLBuffer>>(vertex_buffer.native_handle());

                // get index Buffer handle
                const auto &index_buffer = mesh.buffer_descriptor().index_buffer();
                const auto index_buffer_native = std::any_cast<id<MTLBuffer>>(index_buffer.native_handle());

                auto &cam = camera(entity->camera_type());

                // copy uniform data into a struct
                uniform uniform_data{
                    cam.projection(),
                    cam.view(),
                    entity->transform(),
                    entity->normal_transform(),
                };
                std::memcpy(
                    uniform_data.bones,
                    entity->skeleton().transforms().data(),
                    entity->skeleton().transforms().size() * sizeof(Matrix4));

                static float light[] = { 10.0f, 10.0f, 10.0f, 0.0f };

                // encode render commands
                [render_encoder setRenderPipelineState:pipeline_state];
                [render_encoder setVertexBuffer:vertex_buffer_native offset:0 atIndex:0];
                [render_encoder setVertexBytes:static_cast<const void*>(&uniform_data) length:sizeof(uniform_data) atIndex:1];
                [render_encoder setFragmentBytes:static_cast<const void*>(&light) length:sizeof(light) atIndex:0];
                [render_encoder setCullMode:MTLCullModeNone];

                const auto texture = std::any_cast<id<MTLTexture>>(mesh.texture()->native_handle());
                [render_encoder setFragmentTexture:texture atIndex:0];

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

        // end frame
        [render_encoder endEncoding];
        [command_buffer presentDrawable:drawable];
        [command_buffer commit];
    }
}

RenderEntity* RenderSystem::add(std::unique_ptr<RenderEntity> entity)
{
    scene_.emplace_back(std::move(entity));
    return scene_.back().get();
}

void RenderSystem::remove(RenderEntity *entity)
{
    scene_.erase(
        std::remove_if(
            std::begin(scene_),
            std::end(scene_),
            [entity](const auto &element) { return element.get() == entity; }),
        std::end(scene_));
}

Camera& RenderSystem::persective_camera()
{
    return persective_camera_;
}

Camera& RenderSystem::orthographic_camera()
{
    return orthographic_camera_;
}

Camera& RenderSystem::camera(CameraType type)
{
    switch(type)
    {
        case CameraType::PERSPECTIVE: return persective_camera(); break;
        case CameraType::ORTHOGRAPHIC: return orthographic_camera(); break;
    }
}

}

