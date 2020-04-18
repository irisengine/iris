#include "graphics/render_system.hpp"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>

#include "core/exception.hpp"
#include "core/matrix4.hpp"
#include "core/vector3.hpp"
#include "graphics/buffer.hpp"
#include "graphics/sprite.hpp"
#include "log/log.hpp"

namespace
{

struct uniform
{
    eng::Matrix4 projection;
    eng::Matrix4 view;
    eng::Matrix4 model;
};

}

namespace eng
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
        id<MTLTexture> depth_texture,
        id<MTLDepthStencilState> depth_stencil_state)
    : command_queue(command_queue),
      layer(layer),
      depth_texture(depth_texture),
      depth_stencil_state(depth_stencil_state)
    { }

    /** Command queue for rendering a frame. */
    id<MTLCommandQueue> command_queue;

    /** Pointer to metal layer to render to. */
    CAMetalLayer *layer;

    /** Texture for depth buffer. */
    id<MTLTexture> depth_texture;

    /** Metal state for depth buffer. */
    id<MTLDepthStencilState> depth_stencil_state;
};

RenderSystem::RenderSystem()
    : scene_(),
      camera_(),
      light_position(),
      impl_(nullptr)
{
    // get metal device handle
    const auto *device = ::CGDirectDisplayCopyCurrentMetalDevice(::CGMainDisplayID());
    if(device == nullptr)
    {
        throw Exception("could not get metal device");
    }

    // create a new command queue for rendering
    const auto command_queue = [device newCommandQueue];
    if(command_queue == nullptr)
    {
        throw Exception("could not creare command queue");
    }

    // get a pointer to the main window
    auto *Window = [[NSApp windows] firstObject];
    if(Window == nullptr)
    {
        throw Exception("could not get main window");
    }

    // get a pointer to the metal layer to render to
    auto *layer = static_cast<CAMetalLayer*>([[Window contentView] layer]);
    if(layer == nullptr)
    {
        throw Exception("could not get metal later");
    }

    // get next layer so we can query frame size
    [layer nextDrawable];

    // get frame size
    const auto size = [layer drawableSize];
    const auto scale = [[Window screen] backingScaleFactor];

    // create and setup descriptor for depth texture
    auto *texture_description =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                           width:size.width * scale
                                                          height:size.height * scale
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

    // create implementation struct
    impl_ = std::make_unique<implementation>(
        command_queue,
        layer,
        depth_texture,
        depth_stencil_state);

    LOG_ENGINE_INFO("render_system", "constructed metal render system");
}

/** Default */
RenderSystem::~RenderSystem() = default;
RenderSystem::RenderSystem(RenderSystem&&) = default;
RenderSystem& RenderSystem::operator=(RenderSystem&&) = default;

void RenderSystem::render() const
{
    const auto drawable = [impl_->layer nextDrawable];

    // using an autoreleasepool allows us to release the drawable object as soon
    // as we are done with it and prevents Core Animation from running out of
    // drawables to vend
    @autoreleasepool
    {

    // create and setup a render pass descriptor for this frame
    auto *render_pass_descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setTexture:drawable.texture];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setLoadAction:MTLLoadActionClear];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setClearColor:MTLClearColorMake(0, 0, 0, 0)];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setStoreAction:MTLStoreActionStore];
    [[render_pass_descriptor depthAttachment] setTexture:impl_->depth_texture];
    [[render_pass_descriptor depthAttachment] setClearDepth:1.0f];
    [[render_pass_descriptor depthAttachment] setLoadAction:MTLLoadActionClear];
    [[render_pass_descriptor depthAttachment] setStoreAction:MTLStoreActionDontCare];

    const float light[] = { light_position.x, light_position.y, light_position.z, 1.0f };

    const auto *command_buffer = [impl_->command_queue commandBuffer];

    // create and setup a render encoder
    const auto render_encoder = [command_buffer renderCommandEncoderWithDescriptor:render_pass_descriptor];
    [render_encoder setDepthStencilState:impl_->depth_stencil_state];
    [render_encoder setFrontFacingWinding:MTLWindingCounterClockwise];
    [render_encoder setCullMode:MTLCullModeBack];

    // render scene
    for(const auto &entity : scene_)
    {
        // get pipeline state handle
        const auto pipeline_state = std::any_cast<id<MTLRenderPipelineState>>(entity->material().native_handle());

        [render_encoder setTriangleFillMode:MTLTriangleFillModeFill];

        // set wireframe if needed
        if(entity->should_render_wireframe())
        {
            [render_encoder setTriangleFillMode:MTLTriangleFillModeLines];
        }

        // get vertex Buffer handle
        const auto &vertex_buffer_any = entity->mesh().vertex_buffer();
        const auto vertex_Buffer = std::any_cast<id<MTLBuffer>>(vertex_buffer_any.native_handle());

        // get index Buffer handle
        const auto &index_buffer_any = entity->mesh().index_buffer();
        const auto index_buffer = std::any_cast<id<MTLBuffer>>(index_buffer_any.native_handle());

        // copy uniform data into a struct
        const uniform uniform_data{
            camera_.projection(),
            camera_.view(),
            entity->transform()
        };

        // encode render commands
        [render_encoder setRenderPipelineState:pipeline_state];
        [render_encoder setVertexBuffer:vertex_Buffer offset:0 atIndex:0];
        [render_encoder setVertexBytes:static_cast<const void*>(&uniform_data) length:sizeof(uniform_data) atIndex:1];
        [render_encoder setFragmentBytes:static_cast<const void*>(&light) length:sizeof(light) atIndex:0];

        const auto Texture = std::any_cast<id<MTLTexture>>(entity->mesh().texture().native_handle());
        [render_encoder setFragmentTexture:Texture atIndex:0];

        // draw command
        [render_encoder
            drawIndexedPrimitives:MTLPrimitiveTypeTriangle
            indexCount:entity->mesh().indices().size()
            indexType:MTLIndexTypeUInt32
            indexBuffer:index_buffer
            indexBufferOffset:0];
    }

    // end frame
    [render_encoder endEncoding];
    [command_buffer presentDrawable:drawable];
    [command_buffer commit];

    }
}

Sprite* RenderSystem::add(std::unique_ptr<Sprite> sprite)
{
    scene_.emplace_back(std::move(sprite));
    return scene_.back().get();
}

void RenderSystem::set_light_position(const Vector3 &position)
{
    light_position = position;

    LOG_ENGINE_INFO("render_system", "light position set: {}", light_position);
}

}

