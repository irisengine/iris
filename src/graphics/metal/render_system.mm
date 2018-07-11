#include "render_system.hpp"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>

#include "buffer.hpp"
#include "entity.hpp"
#include "exception.hpp"
#include "log.hpp"
#include "material.hpp"
#include "matrix4.hpp"
#include "vector3.hpp"

namespace
{

// hard coded shaders

static const std::string vertex_source { R"(
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

typedef struct
{
    float4 position;
    float4 normal;
    float4 color;
    float4 tex;
} VertexIn;

typedef struct
{
    float4x4 projection;
    float4x4 view;
    float4x4 model;
} DefaultUniform;

typedef struct
{
    float4 position;
} LightUniform;

typedef struct {
    float4 position [[position]];
    float4 normal [[normal]];
    float4 color;
    float4 tex;
    float4 pos;
} VertexOut;

vertex VertexOut vertex_main(
    device VertexIn *vertices [[buffer(0)]],
    constant DefaultUniform *uniform [[buffer(1)]],
    constant LightUniform *light [[buffer(2)]],
    uint vid [[vertex_id]])
{
    VertexOut out;

    out.position = transpose(uniform->projection) * transpose(uniform->view) * transpose(uniform->model) * vertices[vid].position;
    out.pos = transpose(uniform->model) * vertices[vid].position;
    out.normal = transpose(uniform->model) * vertices[vid].normal;
    out.color = vertices[vid].color;
    out.tex = vertices[vid].tex;
    return out;
}
)" };

static const std::string fragment_source { R"(

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

typedef struct {
    float4 position [[position]];
    float4 normal [[normal]];
    float4 color;
    float4 tex;
    float4 pos;
} VertexOut;

typedef struct
{
    float4x4 projection;
    float4x4 view;
    float4x4 model;
} DefaultUniform;

typedef struct
{
    float4 position;
} LightUniform;

fragment float4 fragment_main(
    VertexOut in [[stage_in]],
    constant LightUniform *light [[buffer(0)]],
    texture2d<float> main_texture [[texture(0)]]
    )
{
    constexpr sampler s(coord::normalized, address::repeat, filter::linear);
    float4 sampled_colour = main_texture.sample(s, in.tex.xy);

    float4 amb(0.2f, 0.2f, 0.2f, 1.0f);
    amb *= 4.0f;
    float4 light_colour(1.0f, 1.0f, 1.0f, 1.0f);
    float4 n = normalize(in.normal);
    float4 light_dir = normalize(light->position - in.pos);
    float diff = max(dot(n, light_dir), 0.0);
    float4 diffuse = light_colour * diff;
    float4 l = amb + diffuse;

    return l * in.color * sampled_colour;
}
)" };

struct uniform
{
    eng::matrix4 projection;
    eng::matrix4 view;
    eng::matrix4 model;
};

}

namespace eng
{

/**
 * Struct containing implementation specific data.
 */
struct render_system::implementation final
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

    /** Default */
    ~implementation() = default;
    implementation(const implementation&) = default;
    implementation& operator=(const implementation&) = default;
    implementation(implementation&&) = default;
    implementation& operator=(implementation&&) = default;

    /** Command queue for rendering a frame. */
    id<MTLCommandQueue> command_queue;

    /** Pointer to metal layer to render to. */
    CAMetalLayer *layer;

    /** Texture for depth buffer. */
    id<MTLTexture> depth_texture;

    /** Metal state for depth buffer. */
    id<MTLDepthStencilState> depth_stencil_state;
};

render_system::render_system(
            std::shared_ptr<camera> cam,
            std::shared_ptr<window> win)
    : scene_(),
      camera_(cam),
      window_(win),
      material_(vertex_source, fragment_source),
      light_position(),
      impl_(nullptr)
{
    // get metal device handle
    const auto *device = ::CGDirectDisplayCopyCurrentMetalDevice(::CGMainDisplayID());
    if(device == nullptr)
    {
        throw exception("could not get metal device");
    }

    // create a new command queue for rendering
    const auto command_queue = [device newCommandQueue];
    if(command_queue == nullptr)
    {
        throw exception("could not creare command queue");
    }

    // get a pointer to the main window
    auto *window = [[NSApp windows] firstObject];
    if(window == nullptr)
    {
        throw exception("could not get main window");
    }

    // get a pointer to the metal layer to render to
    auto *layer = static_cast<CAMetalLayer*>([[window contentView] layer]);
    if(layer == nullptr)
    {
        throw exception("could not get metal later");
    }

    // get next layer so we can query frame size
    [layer nextDrawable];

    // get frame size
    const auto size = [layer drawableSize];
    const auto scale = [[window screen] backingScaleFactor];

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
render_system::~render_system() = default;
render_system::render_system(render_system&&) = default;
render_system& render_system::operator=(render_system&&) = default;

void render_system::add(std::shared_ptr<entity> e)
{
    scene_.emplace_back(e);

    LOG_ENGINE_INFO("render_system", "adding entity");
}

void render_system::render() const
{
    window_->pre_render();

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
        for(const auto &mesh : entity->meshes())
        {
            // get vertex buffer handle
            const auto &vertex_buffer_any = mesh.vertex_buffer();
            const auto vertex_buffer = std::any_cast<id<MTLBuffer>>(vertex_buffer_any.native_handle());

            // get index buffer handle
            const auto &index_buffer_any = mesh.index_buffer();
            const auto index_buffer = std::any_cast<id<MTLBuffer>>(index_buffer_any.native_handle());

            // copy uniform data into a struct
            const uniform uniform_data{
                camera_->projection(),
                camera_->view(),
                entity->transform()
            };

            // get pipeline state handle
            const auto pipeline_state = std::any_cast<id<MTLRenderPipelineState>>(material_.native_handle());

            // set wireframe if needed
            if(entity->should_render_wireframe())
            {
                [render_encoder setTriangleFillMode:MTLTriangleFillModeLines];
            }

            // encode render commands
            [render_encoder setRenderPipelineState:pipeline_state];
            [render_encoder setVertexBuffer:vertex_buffer offset:0 atIndex:0];
            [render_encoder setVertexBytes:static_cast<const void*>(&uniform_data) length:sizeof(uniform_data) atIndex:1];
            [render_encoder setFragmentBytes:static_cast<const void*>(&light) length:sizeof(light) atIndex:0];

            const auto texture = std::any_cast<id<MTLTexture>>(mesh.tex().native_handle());
            [render_encoder setFragmentTexture:texture atIndex:0];

            // draw command
            [render_encoder
                drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                indexCount:mesh.indices().size()
                indexType:MTLIndexTypeUInt32
                indexBuffer:index_buffer
                indexBufferOffset:0];
        }
    }

    // end frame
    [render_encoder endEncoding];
    [command_buffer presentDrawable:drawable];
    [command_buffer commit];

    }

    window_->post_render();
}

void render_system::set_light_position(const vector3 &position) noexcept
{
    light_position = position;

    LOG_ENGINE_INFO("render_system", "light position set: {}", light_position);
}

}

