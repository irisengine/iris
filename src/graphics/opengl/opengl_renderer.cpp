////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/opengl_renderer.h"

#include <cassert>
#include <deque>
#include <string>
#include <utility>

#include "core/camera.h"
#include "core/error_handling.h"
#include "core/vector3.h"
#include "graphics/constant_buffer_writer.h"
#include "graphics/instanced_entity.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/material_manager.h"
#include "graphics/mesh_manager.h"
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_cube_map.h"
#include "graphics/opengl/opengl_material.h"
#include "graphics/opengl/opengl_mesh.h"
#include "graphics/opengl/opengl_render_target.h"
#include "graphics/opengl/opengl_texture.h"
#include "graphics/opengl/opengl_texture_manager.h"
#include "graphics/render_entity.h"
#include "graphics/render_entity_type.h"
#include "graphics/render_graph/sky_box_node.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/sampler.h"
#include "graphics/single_entity.h"
#include "graphics/texture_manager.h"
#include "graphics/window.h"
#include "graphics/window_manager.h"
#include "log/log.h"

#if defined(IRIS_PLATFORM_WIN32)
#include "graphics/win32/win32_opengl_window.h"
#elif defined(IRIS_PLATFORM_LINUX)
#include "graphics/linux/linux_window.h"
#endif

namespace
{

/**
 * Helper function to setup opengl for a render pass.
 *
 * @param target
 *   RenderTarget for render pass.
 */
void render_setup(const iris::OpenGLFrameBuffer &frame_buffer)
{
    if (frame_buffer.colour_target() == nullptr)
    {
        ::glBindFramebuffer(GL_FRAMEBUFFER, 0u);
    }
    else
    {
        ::glViewport(0, 0, frame_buffer.colour_target()->width(), frame_buffer.colour_target()->height());
        iris::expect(iris::check_opengl_error, "could not set viewport");

        frame_buffer.bind();
    }
}

/**
 * Helper function to draw all meshes in a RenderEntity.
 *
 * @param entity
 *   RenderEntity to draw
 */
void draw_meshes(const iris::RenderEntity *entity)
{
    const auto *mesh = static_cast<const iris::OpenGLMesh *>(entity->mesh());
    mesh->bind();

    const auto type = entity->primitive_type() == iris::PrimitiveType::TRIANGLES ? GL_TRIANGLES : GL_LINES;
    const auto instance_count = (entity->type() == iris::RenderEntityType::INSTANCED)
                                    ? static_cast<const iris::InstancedEntity *>(entity)->instance_count()
                                    : 1u;

    // draw!
    ::glDrawElementsInstanced(type, mesh->element_count(), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(instance_count));
    iris::expect(iris::check_opengl_error, "could not draw triangles");

    mesh->unbind();

    if (entity->should_render_wireframe())
    {
        ::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

/**
 * Helper function to create a SSBO for the global texture table.
 *
 * @return
 *   SSBO with all textures loaded at the correct index.
 */
std::unique_ptr<iris::SSBO> create_texture_table_ssbo(iris::TextureManager &texture_manager)
{
    const auto textures = texture_manager.textures();
    const auto max_index = textures.back()->index();
    auto texture_table = std::make_unique<iris::SSBO>((max_index + 1u) * sizeof(GLuint64), 3u);
    const auto *blank_texture = static_cast<const iris::OpenGLTexture *>(texture_manager.blank_texture());
    auto iter = std::cbegin(textures);

    // write bindless handles into the SSBO
    iris::ConstantBufferWriter writer{*texture_table};
    for (auto i = 0u; i <= max_index; ++i)
    {
        // if a texture exits at the current index we write it in
        if (i == (*iter)->index())
        {
            const auto *opengl_texture = static_cast<const iris::OpenGLTexture *>(*iter);
            writer.write(opengl_texture->bindless_handle());
            ++iter;
        }
        else
        {
            // no texture at current index, so write default texture
            writer.write(blank_texture->bindless_handle());
        }
    }

    return texture_table;
}

/**
 * Helper function to create a SSBO for the global cube map table.
 *
 * @return
 *   SSBO with all cube maps loaded at the correct index.
 */
std::unique_ptr<iris::SSBO> create_cube_map_table_ssbo(iris::TextureManager &texture_manager)
{
    const auto cube_maps = texture_manager.cube_maps();
    const auto max_index = cube_maps.back()->index();
    auto cube_map_table = std::make_unique<iris::SSBO>((max_index + 1u) * sizeof(GLuint64), 4u);
    const auto *blank_cube_map = static_cast<const iris::OpenGLCubeMap *>(texture_manager.blank_cube_map());
    auto iter = std::cbegin(cube_maps);

    // write bindless handles into the SSBO
    iris::ConstantBufferWriter writer{*cube_map_table};
    for (auto i = 0u; i <= max_index; ++i)
    {
        // if a cube map exits at the current index we write it in
        if (i == (*iter)->index())
        {
            const auto *opengl_cube_map = static_cast<const iris::OpenGLCubeMap *>(*iter);
            writer.write(opengl_cube_map->bindless_handle());
            ++iter;
        }
        else
        {
            // no cube map at current index, so write default cube map
            writer.write(blank_cube_map->bindless_handle());
        }
    }

    return cube_map_table;
}

}

namespace iris
{

OpenGLRenderer::OpenGLRenderer(
    WindowManager &window_manager,
    TextureManager &texture_manager,
    MaterialManager &material_manager,
    std::uint32_t width,
    std::uint32_t height)
    : Renderer(material_manager)
    , window_manager_(window_manager)
    , texture_manager_(texture_manager)
    , width_(width)
    , height_(height)
    , bone_data_()
    , light_data_()
{
    ::glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
    expect(check_opengl_error, "could not set clear colour");

    ::glEnable(GL_DEPTH_TEST);
    expect(check_opengl_error, "could not enable depth testing");

    ::glDepthFunc(GL_LEQUAL);
    expect(check_opengl_error, "could not set depth test function");

    LOG_ENGINE_INFO("render_system", "constructed opengl renderer");
}

void OpenGLRenderer::do_set_render_pipeline(std::function<void()> build_queue)
{
    instance_data_.clear();
    pass_frame_buffers_.clear();

    build_queue();

    // loop through all draw commands, for each drawn entity create a uniform object so they can be easily set during
    // render
    for (const auto &command : render_queue_)
    {
        if (command.type() == RenderCommandType::DRAW)
        {
            const auto *render_entity = command.render_entity();

            if (render_entity->type() == RenderEntityType::INSTANCED)
            {
                const auto *instanced_entity = static_cast<const InstancedEntity *>(render_entity);
                instance_data_[render_entity] =
                    std::make_unique<SSBO>(instanced_entity->data().size() * sizeof(Matrix4), 5u);

                ConstantBufferWriter writer{*instance_data_[render_entity]};

                writer.write(instanced_entity->data());
            }
        }
    }

    for (const auto *pass : render_pipeline_->render_passes())
    {
        pass_frame_buffers_[pass] = OpenGLFrameBuffer{
            static_cast<const OpenGLRenderTarget *>(pass->colour_target),
            static_cast<const OpenGLRenderTarget *>(pass->normal_target),
            static_cast<const OpenGLRenderTarget *>(pass->position_target)};
    }

    texture_table_ = create_texture_table_ssbo(texture_manager_);
    cube_map_table_ = create_cube_map_table_ssbo(texture_manager_);
}

void OpenGLRenderer::execute_pass_start(RenderCommand &command)
{
    const auto &frame_buffer = pass_frame_buffers_.at(command.render_pass());
    const auto *camera = command.render_pass()->camera;

    // if we have no target then we render to the default framebuffer
    // else we bind the supplied target
    if (frame_buffer.colour_target() == nullptr)
    {
        const auto scale = window_manager_.current_window()->screen_scale();

        ::glViewport(0, 0, width_ * scale, height_ * scale);
        expect(check_opengl_error, "could not set viewport");

        ::glBindFramebuffer(GL_FRAMEBUFFER, 0);
        expect(check_opengl_error, "could not bind default buffer");
    }
    else
    {
        ::glViewport(0, 0, frame_buffer.colour_target()->width(), frame_buffer.colour_target()->height());
        expect(check_opengl_error, "could not set viewport");

        frame_buffer.bind();
    }

    // clear current target

    if (command.render_pass()->clear_colour)
    {
        ::glClear(GL_COLOR_BUFFER_BIT);
        expect(check_opengl_error, "could not clear");
    }

    if (command.render_pass()->clear_depth)
    {
        ::glClear(GL_DEPTH_BUFFER_BIT);
        expect(check_opengl_error, "could not clear");
    }

    bone_data_.clear();
    model_data_.clear();
    light_data_.clear();

    camera_data_ = std::make_unique<UBO>((sizeof(Matrix4) * 3u) + sizeof(Vector3), 0u);

    // calculate view matrix for normals
    auto normal_view = Matrix4::transpose(Matrix4::invert(camera->view()));
    normal_view[3] = 0.0f;
    normal_view[7] = 0.0f;
    normal_view[11] = 0.0f;

    ConstantBufferWriter writer{*camera_data_};
    writer.write(camera->projection());
    writer.write(camera->view());
    writer.write(normal_view);
    writer.write(camera->position());
}

void OpenGLRenderer::execute_draw(RenderCommand &command)
{
    const auto *render_entity = command.render_entity();
    const auto *light = command.light();
    const auto &frame_buffer = pass_frame_buffers_.at(command.render_pass());

    static const OpenGLRenderTarget *previous_target = nullptr;
    const auto *target = static_cast<const OpenGLRenderTarget *>(frame_buffer.colour_target());

    // optimisation, we only call render_setup when the target changes
    if (target != previous_target)
    {
        render_setup(frame_buffer);
        previous_target = target;
    }

    static const OpenGLMaterial *previous_material = nullptr;
    auto *material = static_cast<const OpenGLMaterial *>(command.material());

    // optimisation, we only bind a material when it changes
    if (material != previous_material)
    {
        material->bind();
        previous_material = material;
    }

    // set blend mode based on light
    // ambient is always rendered first (no blending)
    // directional and point are always rendered after (blending)
    switch (light->type())
    {
        case LightType::AMBIENT: ::glDisable(GL_BLEND); break;
        case LightType::DIRECTIONAL:
        case LightType::POINT:
            ::glEnable(GL_BLEND);
            ::glBlendFunc(GL_ONE, GL_ONE);
            break;
    }

    if (render_entity->should_render_wireframe())
    {
        ::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // we use caching to minimise the CPU->GPU communication
    // the first time we see a RenderEntity we cache its bone data (and if its single entity its transform data) we can
    // then reuse these buffers for any subsequent renders of that entity (in this pass)

    if (!bone_data_.contains(render_entity))
    {
        static std::vector<Matrix4> default_bones(100u);

        // first time seeing this entity this pass, so create a new UBO
        bone_data_[render_entity] = std::make_unique<UBO>(sizeof(Matrix4) * 100u, 1u);

        ConstantBufferWriter writer{*bone_data_[render_entity]};

        if (render_entity->type() == RenderEntityType::SINGLE)
        {
            // a single entity, so write in bone data

            const auto *single_entity = static_cast<const SingleEntity *>(render_entity);

            if (single_entity->skeleton() != nullptr)
            {
                const auto &bones = single_entity->skeleton()->transforms();
                writer.write(bones);
                writer.advance((100u - bones.size()) * sizeof(iris::Matrix4));
            }
            else
            {
                writer.write(default_bones);
            }

            // also cache the entities transform data
            model_data_[render_entity] = std::make_unique<SSBO>(128u, 5u);
            ConstantBufferWriter writer2{*model_data_[render_entity]};
            writer2.write(single_entity->transform());
            writer2.write(single_entity->normal_transform());
        }
        else
        {
            // we don't support animation of instanced entities - so just send default bone transforms
            writer.write(default_bones);
        }
    }

    // we also cache light data per pass
    // the first time we see  alight we write its data to a UBO and reuse it for subsequent renders

    if (!light_data_.contains(light))
    {
        light_data_[light] = std::make_unique<UBO>(256u, 2u);

        ConstantBufferWriter writer{*light_data_[light]};

        if (light->type() == iris::LightType::DIRECTIONAL)
        {
            // directional lights have additional data
            const auto *direction_light = static_cast<const iris::DirectionalLight *>(light);

            writer.write(direction_light->shadow_camera().projection());
            writer.write(direction_light->shadow_camera().view());
        }
        else
        {
            // skip over directional light specific data
            writer.advance(sizeof(iris::Matrix4) * 2u);
        }

        writer.write(light->colour_data());
        writer.write(light->world_space_data());
        const auto attenuation = light->attenuation_data();
        std::array<float, 4u> padded{attenuation[0], attenuation[1], attenuation[2], 0.0f};
        writer.write(padded);
    }

    // if we are rendering with a directional light check if it casts shadows
    if (light->type() == LightType::DIRECTIONAL)
    {
        const auto *directional_light = static_cast<const DirectionalLight *>(light);
        if ((directional_light->casts_shadows() && (command.render_entity()->receive_shadow())))
        {
            // if we are rendering with a shadow casting directional light then pass the index of the shadow map (into
            // the texture table) as a uniform
            OpenGLUniform shadow_map_index_uniform{material->handle(), "shadow_map_index", true};
            shadow_map_index_uniform.set_value(command.shadow_map()->depth_texture()->index());
        }
    }

    ::glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera_data_->handle());
    expect(check_opengl_error, "could not bind camera data ubo");

    ::glBindBufferBase(GL_UNIFORM_BUFFER, 1, bone_data_[render_entity]->handle());
    expect(check_opengl_error, "could not bind vertex data ubo");

    ::glBindBufferBase(GL_UNIFORM_BUFFER, 2, light_data_[light]->handle());
    expect(check_opengl_error, "could not bind light data ubo");

    ::glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, texture_table_->handle());
    expect(check_opengl_error, "could not bind texture data ssbo");

    ::glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, cube_map_table_->handle());
    expect(check_opengl_error, "could not bind cube map data ssbo");

    // bind model data, depending on if we're rendering a single or instanced entity
    if (render_entity->type() != RenderEntityType::INSTANCED)
    {
        ::glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, model_data_[render_entity]->handle());
        expect(check_opengl_error, "could not bind model data ssbo");
    }
    else
    {
        ::glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, instance_data_[render_entity]->handle());
        expect(check_opengl_error, "could not bind model data ssbo");
    }

    draw_meshes(render_entity);
}

void OpenGLRenderer::execute_present(RenderCommand &)
{
#if defined(IRIS_PLATFORM_MACOS)
    ::glSwapAPPLE();
#elif defined(IRIS_PLATFORM_WIN32)
    const auto *window = static_cast<Win32OpenGLWindow *>(window_manager_.current_window());
    ::SwapBuffers(window->device_context());
#elif defined(IRIS_PLATFORM_LINUX)
    const auto *window = static_cast<LinuxWindow *>(window_manager_.current_window());
    ::glXSwapBuffers(window->display(), window->window());
#endif
}
}
