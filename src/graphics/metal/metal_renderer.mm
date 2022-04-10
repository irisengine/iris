////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/metal/metal_renderer.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>
#import <QuartzCore/QuartzCore.h>

#include "core/error_handling.h"
#include "core/macos/macos_ios_utility.h"
#include "core/matrix4.h"
#include "core/root.h"
#include "core/vector3.h"
#include "graphics/constant_buffer_writer.h"
#include "graphics/instanced_entity.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/mesh_manager.h"
#include "graphics/metal/metal_constant_buffer.h"
#include "graphics/metal/metal_cube_map.h"
#include "graphics/metal/metal_material.h"
#include "graphics/metal/metal_mesh.h"
#include "graphics/metal/metal_render_target.h"
#include "graphics/metal/metal_sampler.h"
#include "graphics/metal/metal_texture.h"
#include "graphics/render_entity.h"
#include "graphics/render_graph/post_processing_node.h"
#include "graphics/render_graph/sky_box_node.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/render_queue_builder.h"
#include "graphics/render_target.h"
#include "graphics/sampler.h"
#include "graphics/single_entity.h"
#include "graphics/texture_manager.h"
#include "graphics/window.h"
#include "log/log.h"

namespace
{

// this matrix is used to translate projection matrices from engine NDC to
// metal NDC
static const iris::Matrix4 metal_translate{
    {{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f}}};

/**
 * Helper function to create a render encoder for render pass.
 *
 * @param colour
 *   Metal texture to write colour data to.
 *
 * @param depth
 *   Metal texture to write depth data to.
 *
 * @param descriptor
 *   Descriptor for render pass.
 *
 * @param depth_stencil_state
 *   State for depth/stencil.
 *
 * @param commands_buffer
 *   Command buffer to create encoder from.
 *
 * @returns
 *   RenderCommandEncoder object for a render pass.
 */
id<MTLRenderCommandEncoder> create_render_encoder(
    id<MTLTexture> colour,
    id<MTLTexture> depth,
    MTLRenderPassDescriptor *descriptor,
    const id<MTLDepthStencilState> depth_stencil_state,
    id<MTLCommandBuffer> command_buffer)
{
    descriptor.colorAttachments[0].texture = colour;
    descriptor.depthAttachment.texture = depth;

    const auto render_encoder = [command_buffer renderCommandEncoderWithDescriptor:descriptor];
    [render_encoder setDepthStencilState:depth_stencil_state];
    [render_encoder setFrontFacingWinding:MTLWindingCounterClockwise];
    [render_encoder setCullMode:MTLCullModeBack];
    [render_encoder setTriangleFillMode:MTLTriangleFillModeFill];

    return render_encoder;
}

/**
 * Helper function to build the texture table - a global GPU buffer of all textures (used for bindless rendering)
 *
 * @returns
 *   Metal buffer with all loaded textures.
 */
std::unique_ptr<iris::MetalConstantBuffer> create_texture_table()
{
    const auto *device = iris::core::utility::metal_device();

    const auto *blank_texture = static_cast<const iris::MetalTexture *>(iris::Root::texture_manager().blank_texture());
    const auto textures = iris::Root::texture_manager().textures();
    const auto max_index = textures.back()->index();
    auto iter = std::cbegin(textures);

    // create descriptor for arguments we want to write
    auto *argument_descriptor = [MTLArgumentDescriptor argumentDescriptor];
    [argument_descriptor setIndex:0];
    [argument_descriptor setDataType:MTLDataTypeTexture];
    [argument_descriptor setAccess:MTLArgumentAccessReadOnly];
    [argument_descriptor setTextureType:MTLTextureType2D];
    [argument_descriptor setArrayLength:max_index + 1u];

    auto *argument_descriptors = [NSArray arrayWithObjects:argument_descriptor, nil];

    // create encoder to actually set arguments
    auto texture_table_argument_encoder = [device newArgumentEncoderWithArguments:argument_descriptors];

    // create table buffer
    auto texture_table =
        std::make_unique<iris::MetalConstantBuffer>([texture_table_argument_encoder encodedLength] * (max_index + 1u));
    [texture_table_argument_encoder setArgumentBuffer:texture_table->handle() offset:0];

    for (auto i = 0u; i <= max_index; ++i)
    {
        // if a texture exits at the current index we write it in
        if (i == (*iter)->index())
        {
            const auto *metal_texture = static_cast<const iris::MetalTexture *>(*iter);
            [texture_table_argument_encoder setTexture:metal_texture->handle() atIndex:i];
            ++iter;
        }
        else
        {
            // no texture at current index, so write default texture
            [texture_table_argument_encoder setTexture:blank_texture->handle() atIndex:i];
        }
    }

    return texture_table;
}

/**
 * Helper function to build the cube map table - a global GPU buffer of all cube maps (used for bindless rendering)
 *
 * @returns
 *   Metal buffer with all loaded cube map.
 */
std::unique_ptr<iris::MetalConstantBuffer> create_cube_map_table()
{
    const auto *device = iris::core::utility::metal_device();

    const auto *blank_cube_map =
        static_cast<const iris::MetalCubeMap *>(iris::Root::texture_manager().blank_cube_map());
    const auto cube_maps = iris::Root::texture_manager().cube_maps();
    const auto max_index = cube_maps.back()->index();
    auto iter = std::cbegin(cube_maps);

    // create descriptor for arguments we want to write
    auto *argument_descriptor = [MTLArgumentDescriptor argumentDescriptor];
    [argument_descriptor setIndex:0];
    [argument_descriptor setDataType:MTLDataTypeTexture];
    [argument_descriptor setAccess:MTLArgumentAccessReadOnly];
    [argument_descriptor setTextureType:MTLTextureTypeCube];
    [argument_descriptor setArrayLength:max_index + 1u];

    auto *argument_descriptors = [NSArray arrayWithObjects:argument_descriptor, nil];

    // create encoder to actually set arguments
    auto cube_map_table_argument_encoder = [device newArgumentEncoderWithArguments:argument_descriptors];

    // create table buffer
    auto cube_map_table =
        std::make_unique<iris::MetalConstantBuffer>([cube_map_table_argument_encoder encodedLength] * (max_index + 1u));
    [cube_map_table_argument_encoder setArgumentBuffer:cube_map_table->handle() offset:0];

    for (auto i = 0u; i <= max_index; ++i)
    {
        // if a cube map exits at the current index we write it in
        if (i == (*iter)->index())
        {
            const auto *metal_cube_map = static_cast<const iris::MetalCubeMap *>(*iter);
            [cube_map_table_argument_encoder setTexture:metal_cube_map->handle() atIndex:i];
            ++iter;
        }
        else
        {
            // no cube map at current index, so write default cube map
            [cube_map_table_argument_encoder setTexture:blank_cube_map->handle() atIndex:i];
        }
    }

    return cube_map_table;
}

/**
 * Helper function to build the sampler table - a global GPU buffer of all samplers (used for bindless rendering)
 *
 * @returns
 *   Metal buffer with all loaded samplers.
 */
std::unique_ptr<iris::MetalConstantBuffer> create_sampler_table()
{
    const auto *device = iris::core::utility::metal_device();

    const auto samplers = iris::Root::texture_manager().samplers();
    const auto max_index = samplers.back()->index();
    const auto *default_sampler =
        static_cast<const iris::MetalSampler *>(iris::Root::texture_manager().default_texture_sampler());
    auto iter = std::cbegin(samplers);

    // create descriptor for arguments we want to write
    auto *argument_descriptor = [MTLArgumentDescriptor argumentDescriptor];
    [argument_descriptor setIndex:0];
    [argument_descriptor setDataType:MTLDataTypeSampler];
    [argument_descriptor setAccess:MTLArgumentAccessReadOnly];
    [argument_descriptor setArrayLength:max_index + 1u];

    auto *argument_descriptors = [NSArray arrayWithObjects:argument_descriptor, nil];

    // create encoder to actually set arguments
    auto sampler_table_argument_encoder = [device newArgumentEncoderWithArguments:argument_descriptors];

    // create table buffer
    auto sampler_table =
        std::make_unique<iris::MetalConstantBuffer>([sampler_table_argument_encoder encodedLength] * (max_index + 1u));
    [sampler_table_argument_encoder setArgumentBuffer:sampler_table->handle() offset:0];

    for (auto i = 0u; i <= max_index; ++i)
    {
        // if a sampler exits at the current index we write it in
        if (i == (*iter)->index())
        {
            const auto *metal_sampler = static_cast<const iris::MetalSampler *>(*iter);
            [sampler_table_argument_encoder setSamplerState:metal_sampler->handle() atIndex:i];
            ++iter;
        }
        else
        {
            // no sampler at current index, so write default sampler
            [sampler_table_argument_encoder setSamplerState:default_sampler->handle() atIndex:i];
        }
    }

    return sampler_table;
}

}

namespace iris
{

MetalRenderer::MetalRenderer(std::uint32_t width, std::uint32_t height)
    : Renderer()
    , width_(width)
    , height_(height)
    , command_queue_()
    , descriptor_()
    , drawable_()
    , command_buffer_()
    , depth_stencil_state_()
    , render_encoder_()
    , current_frame_(0u)
    , frames_()
    , render_encoders_()
    , render_targets_()
    , materials_()
    , default_depth_buffer_()
    , instance_data_()
    , camera_data_()
    , texture_table_()
    , cube_map_table_()
    , sampler_table_()
{
    const auto *device = iris::core::utility::metal_device();

    ensure(
        [device argumentBuffersSupport] == MTLArgumentBuffersTier::MTLArgumentBuffersTier2,
        "need tier 2 argument buffer support");

    command_queue_ = [device newCommandQueue];
    ensure(command_queue_ != nullptr, "could not create command queue");

    const auto scale = 2;

    // create and setup descriptor for depth texture
    auto *texture_description = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                                                   width:width * scale
                                                                                  height:height * scale
                                                                               mipmapped:NO];
    [texture_description setResourceOptions:MTLResourceStorageModePrivate];
    [texture_description setUsage:MTLTextureUsageRenderTarget];

    // create descriptor for depth checking
    auto *depth_stencil_descriptor = [MTLDepthStencilDescriptor new];
    [depth_stencil_descriptor setDepthCompareFunction:MTLCompareFunctionLessEqual];
    [depth_stencil_descriptor setDepthWriteEnabled:YES];

    // create depth state
    depth_stencil_state_ = [device newDepthStencilStateWithDescriptor:depth_stencil_descriptor];

    // create and setup a render pass descriptor
    descriptor_ = [MTLRenderPassDescriptor renderPassDescriptor];
    [[[descriptor_ colorAttachments] objectAtIndexedSubscript:0] setLoadAction:MTLLoadActionClear];
    [[[descriptor_ colorAttachments] objectAtIndexedSubscript:0]
        setClearColor:MTLClearColorMake(0.39f, 0.58f, 0.93f, 1.0f)];
    [[[descriptor_ colorAttachments] objectAtIndexedSubscript:0] setStoreAction:MTLStoreActionStore];
    [[[descriptor_ colorAttachments] objectAtIndexedSubscript:0] setTexture:nullptr];
    [[descriptor_ depthAttachment] setTexture:nullptr];
    [[descriptor_ depthAttachment] setClearDepth:1.0f];
    [[descriptor_ depthAttachment] setLoadAction:MTLLoadActionClear];
    [[descriptor_ depthAttachment] setStoreAction:MTLStoreActionStore];

    const auto *rt_sampler = Root::texture_manager().create(SamplerDescriptor{.uses_mips = false});

    // create default depth buffer
    default_depth_buffer_ = static_cast<const MetalTexture *>(
        Root::texture_manager().create(DataBuffer{}, width * scale, height * scale, TextureUsage::DEPTH, rt_sampler));
    render_encoder_ = nullptr;
}

MetalRenderer::~MetalRenderer()
{
    // we need to wait for any inflight frames to have finished being rendered
    // by the GPU before we can destruct
    // this line very deliberately creates an unnamed scoped_lock, this
    // ensures the locks for all frames will be waited on, acquired and then
    // immediately released
    std::scoped_lock{frames_[0].lock, frames_[1].lock, frames_[2].lock};
}

void MetalRenderer::set_render_passes(const std::vector<RenderPass> &render_passes)
{
    materials_.clear();
    render_passes_ = render_passes;

    // add a post processing pass

    // find the pass which renders to the screen
    auto final_pass = std::find_if(
        std::begin(render_passes_),
        std::end(render_passes_),
        [](const RenderPass &pass) { return pass.render_target == nullptr; });

    ensure(final_pass != std::cend(render_passes_), "no final pass");

    // deferred creating of render target to ensure this class is full
    // constructed
    if (post_processing_target_ == nullptr)
    {
        post_processing_target_ = create_render_target(width_, height_);
        post_processing_camera_ = std::make_unique<Camera>(CameraType::ORTHOGRAPHIC, width_, height_);
    }

    post_processing_scene_ = std::make_unique<Scene>();

    // create a full screen quad which renders the final stage with the post
    // processing node
    auto *rg = post_processing_scene_->create_render_graph();
    rg->set_render_node<PostProcessingNode>(rg->create<TextureNode>(post_processing_target_->colour_texture()));
    post_processing_scene_->create_entity<SingleEntity>(
        rg,
        Root::mesh_manager().sprite({}),
        Transform({}, {}, {static_cast<float>(width_), static_cast<float>(height_), 1.0}));

    // wire up this pass
    final_pass->render_target = post_processing_target_;
    render_passes_.push_back({.scene = post_processing_scene_.get(), .camera = post_processing_camera_.get()});

    // build the render queue from the provided passes

    RenderQueueBuilder queue_builder(
        [this](RenderGraph *render_graph, RenderEntity *entity, const RenderTarget *target, LightType light_type)
        {
            if (materials_.count(render_graph) == 0u || materials_[render_graph].count(light_type) == 0u)
            {
                materials_[render_graph][light_type] = std::make_unique<MetalMaterial>(
                    render_graph, static_cast<const MetalMesh *>(entity->mesh())->descriptors(), light_type);
            }

            return materials_[render_graph][light_type].get();
        },
        [this](std::uint32_t width, std::uint32_t height) { return create_render_target(width, height); });
    render_queue_ = queue_builder.build(render_passes_);

    instance_data_.clear();

    for (auto &frame : frames_)
    {
        frame.model_data.clear();
    }

    // find all instanced render entities and write their instance data to buffers - this allows us to quickly set them
    // during rendering
    for (const auto &command : render_queue_)
    {
        if (command.type() == RenderCommandType::DRAW)
        {
            const auto *render_entity = command.render_entity();

            if (render_entity->instance_count() > 1u)
            {
                const auto *instanced_entity = static_cast<const InstancedEntity *>(render_entity);
                instance_data_[render_entity] =
                    std::make_unique<MetalConstantBuffer>(instanced_entity->data().size() * sizeof(Matrix4) * 2u);

                ConstantBufferWriter writer{*instance_data_[render_entity]};

                writer.write(instanced_entity->data());
            }
        }
    }

    texture_table_ = create_texture_table();
    cube_map_table_ = create_cube_map_table();
    sampler_table_ = create_sampler_table();
}

RenderTarget *MetalRenderer::create_render_target(std::uint32_t width, std::uint32_t height)
{
    const auto scale = Root::window_manager().current_window()->screen_scale();
    const auto *rt_sampler = Root::texture_manager().create(SamplerDescriptor{.uses_mips = false});
    const auto *depth_sampler = Root::texture_manager().create(SamplerDescriptor{
        .uses_mips = false,
        .s_address_mode = SamplerAddressMode::CLAMP_TO_BORDER,
        .t_address_mode = SamplerAddressMode::CLAMP_TO_BORDER,
        .border_colour = Colour{1.0f, 1.0f, 1.0f, 1.0f}});

    render_targets_.emplace_back(std::make_unique<MetalRenderTarget>(
        static_cast<MetalTexture *>(Root::texture_manager().create(
            DataBuffer{}, width * scale, height * scale, TextureUsage::RENDER_TARGET, rt_sampler)),
        static_cast<MetalTexture *>(Root::texture_manager().create(
            DataBuffer{}, width * scale, height * scale, TextureUsage::DEPTH, depth_sampler))));

    return render_targets_.back().get();
}

void MetalRenderer::pre_render()
{
    @autoreleasepool
    {
        // acquire the lock, this will be released when the GPU has finished rendering the frame
        frames_[current_frame_ % 3u].lock.lock();

        const auto layer = core::utility::metal_layer();
#if defined(IRIS_PLATFORM_MACOS)
        // can only disable vsync on macos
        [layer setDisplaySyncEnabled:NO];
#endif
        drawable_ = [layer nextDrawable];
        command_buffer_ = [command_queue_ commandBuffer];

        // create render encoders fresh each frame
        render_encoders_.clear();
    }
}

void MetalRenderer::execute_pass_start(RenderCommand &command)
{
    const auto *camera = command.render_pass()->camera;

    // only need to set camera data once per pass
    camera_data_ = std::make_unique<MetalConstantBuffer>(sizeof(Matrix4) + sizeof(Matrix4) + (sizeof(float) * 4u));

    ConstantBufferWriter writer{*camera_data_};
    writer.write(metal_translate * camera->projection());
    writer.write(camera->view());
    writer.write(camera->position());

    const auto frame_index = current_frame_ % 3u;
    auto &frame = frames_[frame_index];

    frame.bone_data.clear();
    frame.light_data.clear();
}

void MetalRenderer::execute_draw(RenderCommand &command)
{
    const auto *material = static_cast<const MetalMaterial *>(command.material());
    const auto *entity = command.render_entity();
    const auto *mesh = static_cast<const MetalMesh *>(entity->mesh());
    const auto *target = command.render_pass()->render_target;
    const auto *light = command.light();
    auto &frame = frames_[current_frame_ % 3u];

    @autoreleasepool
    {
        // create a render encoder based on the render command target
        if (render_encoders_.count(target) == 0u)
        {
            id<MTLRenderCommandEncoder> encoder;
            if (target == nullptr)
            {
                // no target means render to the window frame buffer
                encoder = create_render_encoder(
                    drawable_.texture,
                    default_depth_buffer_->handle(),
                    descriptor_,
                    depth_stencil_state_,
                    command_buffer_);
            }
            else
            {
                encoder = create_render_encoder(
                    static_cast<const MetalTexture *>(target->colour_texture())->handle(),
                    static_cast<const MetalTexture *>(target->depth_texture())->handle(),
                    descriptor_,
                    depth_stencil_state_,
                    command_buffer_);
            }

            render_encoders_[target] = encoder;
        }
    }

    render_encoder_ = render_encoders_[target];

    // if we haven't seen this entity yet this pass then create buffers for its data
    if (!frame.bone_data.contains(entity))
    {
        frame.bone_data[entity] = std::make_unique<MetalConstantBuffer>(sizeof(Matrix4) * 100u);
        frame.model_data[entity] = std::make_unique<MetalConstantBuffer>(sizeof(Matrix4) * 2u);

        ConstantBufferWriter writer{*frame.bone_data[entity]};

        if (entity->instance_count() == 1u)
        {
            const auto *single_entity = static_cast<const SingleEntity *>(entity);
            const auto &bones = single_entity->skeleton().transforms();
            writer.write(bones);
            writer.advance((100u - bones.size()) * sizeof(iris::Matrix4));

            iris::ConstantBufferWriter writer2(*frame.model_data[entity]);
            writer2.write(single_entity->transform());
            writer2.write(single_entity->normal_transform());
        }
        else
        {
            static std::vector<iris::Matrix4> default_bones(100u);
            writer.write(default_bones);
        }
    }

    // if we haven't seen this light yet this pass then create buffers for its data
    if (!frame.light_data.contains(light))
    {
        frame.light_data[light] = std::make_unique<MetalConstantBuffer>(176u);

        ConstantBufferWriter writer{*frame.light_data[light]};
        writer.write(light->colour_data());
        writer.write(light->world_space_data());
        writer.write(light->attenuation_data());
        writer.write(0.0f);

        if (light->type() == LightType::DIRECTIONAL)
        {
            const auto *directional_light = static_cast<const iris::DirectionalLight *>(light);
            writer.write(metal_translate * directional_light->shadow_camera().projection());
            writer.write(directional_light->shadow_camera().view());
        }
    }

    const auto shadow_map_index =
        static_cast<int>((command.shadow_map() == nullptr) ? 0u : command.shadow_map()->depth_texture()->index());
    const auto shadow_map_sampler_index = static_cast<int>(
        (command.shadow_map() == nullptr) ? 0u : command.shadow_map()->depth_texture()->sampler()->index());

    auto *model_buffer = entity->instance_count() == 1u ? frame.model_data[entity].get() : instance_data_[entity].get();

    [render_encoder_ setVertexBuffer:frame.bone_data[entity]->handle() offset:0 atIndex:1];
    [render_encoder_ setVertexBuffer:camera_data_->handle() offset:0 atIndex:2];
    [render_encoder_ setVertexBuffer:frame.light_data[light]->handle() offset:0 atIndex:3];
    [render_encoder_ setVertexBuffer:model_buffer->handle() offset:0 atIndex:4];

    [render_encoder_ setFragmentBuffer:camera_data_->handle() offset:0 atIndex:0];
    [render_encoder_ setFragmentBuffer:frame.light_data[light]->handle() offset:0 atIndex:1];
    [render_encoder_ setFragmentBuffer:texture_table_->handle() offset:0 atIndex:2];
    [render_encoder_ setFragmentBuffer:cube_map_table_->handle() offset:0 atIndex:3];
    [render_encoder_ setFragmentBytes:&shadow_map_index length:sizeof(shadow_map_index) atIndex:4];
    [render_encoder_ setFragmentBuffer:sampler_table_->handle() offset:0 atIndex:5];
    [render_encoder_ setFragmentBytes:&shadow_map_sampler_index length:sizeof(shadow_map_sampler_index) atIndex:6];

    const auto &vertex_buffer = mesh->vertex_buffer();
    const auto &index_buffer = mesh->index_buffer();

    // encode render commands
    [render_encoder_ setRenderPipelineState:material->pipeline_state()];
    [render_encoder_ setVertexBuffer:vertex_buffer.handle() offset:0 atIndex:0];
    [render_encoder_ setCullMode:MTLCullModeNone];

    const auto type =
        entity->primitive_type() == iris::PrimitiveType::TRIANGLES ? MTLPrimitiveTypeTriangle : MTLPrimitiveTypeLine;

    // draw command
    [render_encoder_ drawIndexedPrimitives:type
                                indexCount:index_buffer.element_count()
                                 indexType:MTLIndexTypeUInt32
                               indexBuffer:index_buffer.handle()
                         indexBufferOffset:0
                             instanceCount:entity->instance_count()];
}

void MetalRenderer::execute_pass_end(RenderCommand &)
{
    // end encoding for pass
    [render_encoder_ endEncoding];
}

void MetalRenderer::execute_present(RenderCommand &)
{
    [command_buffer_ presentDrawable:drawable_];

    // store local copy of frame so it can be correctly accessed via the completion handler
    const auto frame = current_frame_ % 3u;

    // set completion handler for command buffer, when the GPU is finished rendering it will fire the handler, which
    // will unlock the lock, allowing the frame to be rendered to again
    [command_buffer_ addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
      frames_[frame].lock.unlock();
    }];

    [command_buffer_ commit];
}

void MetalRenderer::post_render()
{
    ++current_frame_;
}

}
