#include "graphics/render_system.h"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>

#include "core/exception.h"
#include "core/matrix4.h"
#include "core/vector3.h"
#include "graphics/buffer.h"
#include "graphics/sprite.h"
#include "log/log.h"
#include "platform/macos/macos_ios_utility.h"

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
        MTLRenderPassDescriptor *descriptor)
    : command_queue(command_queue),
      layer(layer),
      descriptor(descriptor)
    { }

    /** Command queue for rendering a frame. */
    id<MTLCommandQueue> command_queue;

    /** Pointer to metal layer to render to. */
    CAMetalLayer *layer;
    
    /** Pointer to render descriptor. */
    MTLRenderPassDescriptor *descriptor;
};

RenderSystem::RenderSystem(float width, float height)
    : scene_(),
      camera_(width, height),
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

    // create and setup a render pass descriptor
    auto *render_pass_descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setLoadAction:MTLLoadActionClear];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setClearColor:MTLClearColorMake(0, 0, 0, 1)];
    [[[render_pass_descriptor colorAttachments] objectAtIndexedSubscript:0] setStoreAction:MTLStoreActionStore];
    
    // create implementation struct
    impl_ = std::make_unique<implementation>(
        command_queue,
        platform::utility::metal_layer(),
        render_pass_descriptor);
}

/** Default */
RenderSystem::~RenderSystem() = default;
RenderSystem::RenderSystem(RenderSystem&&) = default;
RenderSystem& RenderSystem::operator=(RenderSystem&&) = default;

void RenderSystem::render() const
{
    // using an autoreleasepool allows us to release the drawable object as soon
    // as we are done with it and prevents Core Animation from running out of
    // drawables to vend
    @autoreleasepool
    {
        // update descriptor to render to next frame
        const auto drawable = [impl_->layer nextDrawable];
        [[[impl_->descriptor colorAttachments] objectAtIndexedSubscript:0] setTexture:drawable.texture];
      
        const auto *command_buffer = [impl_->command_queue commandBuffer];

        const auto render_encoder = [command_buffer renderCommandEncoderWithDescriptor:impl_->descriptor];

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

            const auto texture = std::any_cast<id<MTLTexture>>(entity->mesh().texture().native_handle());
            [render_encoder setFragmentTexture:texture atIndex:0];

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

}

