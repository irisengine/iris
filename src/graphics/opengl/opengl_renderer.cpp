////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/opengl_renderer.h"

#include <cassert>
#include <string>
#include <utility>

#include "core/camera.h"
#include "core/error_handling.h"
#include "core/root.h"
#include "core/vector3.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/mesh_manager.h"
#include "graphics/opengl/default_uniforms.h"
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_material.h"
#include "graphics/opengl/opengl_mesh.h"
#include "graphics/opengl/opengl_render_target.h"
#include "graphics/opengl/opengl_texture.h"
#include "graphics/opengl/opengl_texture_manager.h"
#include "graphics/render_entity.h"
#include "graphics/render_graph/post_processing_node.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/render_queue_builder.h"
#include "graphics/texture_manager.h"
#include "graphics/window.h"
#include "graphics/window_manager.h"
#include "log/log.h"

#if defined(IRIS_PLATFORM_WIN32)
#include "graphics/win32/win32_opengl_window.h"
#endif

namespace
{

/**
 * Helper function to setup opengl for a render pass.
 *
 * @param target
 *   RenderTarget for render pass.
 */
void render_setup(const iris::OpenGLRenderTarget *target)
{
    if (target == nullptr)
    {
        ::glBindFramebuffer(GL_FRAMEBUFFER, 0u);
    }
    else
    {
        ::glViewport(0, 0, target->colour_texture()->width(), target->colour_texture()->height());
        iris::expect(iris::check_opengl_error, "could not set viewport");

        target->bind(GL_FRAMEBUFFER);
    }

    // clear current target
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * Helper function to set uniforms for a render pass.
 *
 * @param uniforms
 *   Uniforms to set.
 *
 * @param camera
 *   Camera for current render pass.
 *
 * @param entity
 *   Entity being rendered.
 *
 * @param shadow_map
 *   RenderTarget for shadow map, maybe nullptr.
 *
 * @param light
 *   Light effecting entity.
 */
void set_uniforms(
    const iris::DefaultUniforms *uniforms,
    const iris::Camera *camera,
    const iris::RenderEntity *entity,
    const iris::RenderTarget *shadow_map,
    const iris::Light *light)
{
    uniforms->projection.set_value(camera->projection());
    uniforms->view.set_value(camera->view());
    uniforms->model.set_value(entity->transform());
    uniforms->normal_matrix.set_value(entity->normal_transform());
    uniforms->light_colour.set_value(light->colour_data());
    uniforms->light_position.set_value(light->world_space_data());
    uniforms->light_attenuation.set_value(light->attenuation_data());

    // set shadow map specific texture and uniforms, if it was provided
    if ((shadow_map != nullptr) && (light->type() == iris::LightType::DIRECTIONAL))
    {
        const auto *directional_light = static_cast<const iris::DirectionalLight *>(light);
        const auto *opengl_texture = static_cast<const iris::OpenGLTexture *>(shadow_map->depth_texture());
        const auto tex_handle = opengl_texture->handle();

        ::glActiveTexture(opengl_texture->id());
        iris::expect(iris::check_opengl_error, "could not activate texture");

        ::glBindTexture(GL_TEXTURE_2D, tex_handle);
        iris::expect(iris::check_opengl_error, "could not bind texture``");

        uniforms->shadow_map.set_value(opengl_texture->id() - GL_TEXTURE0);
        uniforms->light_projection.set_value(directional_light->shadow_camera().projection());
        uniforms->light_view.set_value(directional_light->shadow_camera().view());
    }

    uniforms->bones.set_value(entity->skeleton().transforms());
}

/**
 * Helper function to bind all textures for a material.
 *
 * @param uniforms
 *   Uniforms to set.
 *
 * @param material
 *   Material to bind textures for.
 */
void bind_textures(const iris::DefaultUniforms *uniforms, const iris::OpenGLMaterial *material)
{
    const auto textures = material->textures();
    for (auto i = 0u; i < textures.size(); ++i)
    {
        const auto *opengl_texture = static_cast<const iris::OpenGLTexture *>(textures[i]);
        const auto tex_handle = opengl_texture->handle();

        ::glActiveTexture(opengl_texture->id());
        iris::expect(iris::check_opengl_error, "could not activate texture");

        ::glBindTexture(GL_TEXTURE_2D, tex_handle);
        iris::expect(iris::check_opengl_error, "could not bind texture``");

        uniforms->textures[i].set_value(opengl_texture->id() - GL_TEXTURE0);
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
    const auto *mesh = static_cast<iris::OpenGLMesh *>(entity->mesh());
    mesh->bind();

    const auto type = entity->primitive_type() == iris::PrimitiveType::TRIANGLES ? GL_TRIANGLES : GL_LINES;

    // draw!
    ::glDrawElements(type, mesh->element_count(), GL_UNSIGNED_INT, 0);
    iris::expect(iris::check_opengl_error, "could not draw triangles");

    mesh->unbind();

    if (entity->should_render_wireframe())
    {
        ::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

}

namespace iris
{

OpenGLRenderer::OpenGLRenderer(std::uint32_t width, std::uint32_t height)
    : Renderer()
    , render_targets_()
    , materials_()
    , uniforms_()
    , width_(width)
    , height_(height)
{
    ::glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
    expect(check_opengl_error, "could not set clear colour");

    ::glEnable(GL_DEPTH_TEST);
    expect(check_opengl_error, "could not enable depth testing");

    ::glDepthFunc(GL_LEQUAL);
    expect(check_opengl_error, "could not set depth test function");

    LOG_ENGINE_INFO("render_system", "constructed opengl renderer");
}

void OpenGLRenderer::set_render_passes(const std::vector<RenderPass> &render_passes)
{
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
    post_processing_scene_->create_entity(
        rg,
        Root::mesh_manager().sprite({}),
        Transform({}, {}, {static_cast<float>(width_), static_cast<float>(height_), 1.0}));

    // wire up this pass
    final_pass->render_target = post_processing_target_;
    render_passes_.emplace_back(post_processing_scene_.get(), post_processing_camera_.get(), nullptr);

    // build the render queue from the provided passes

    RenderQueueBuilder queue_builder(
        [this](RenderGraph *render_graph, RenderEntity *, const RenderTarget *, LightType light_type)
        {
            if (materials_.count(render_graph) == 0u || materials_[render_graph].count(light_type) == 0u)
            {
                materials_[render_graph][light_type] = std::make_unique<OpenGLMaterial>(render_graph, light_type);
            }

            return materials_[render_graph][light_type].get();
        },
        [this](std::uint32_t width, std::uint32_t height) { return create_render_target(width, height); });

    render_queue_ = queue_builder.build(render_passes_);

    uniforms_.clear();

    // loop through all draw commands, for each drawn entity create a uniform
    // object so they can be esily set during render
    for (const auto &command : render_queue_)
    {
        if (command.type() == RenderCommandType::DRAW)
        {
            const auto *material = static_cast<const OpenGLMaterial *>(command.material());
            const auto *render_entity = command.render_entity();

            // we store uniforms per entity per material
            if (!uniforms_[material][render_entity])
            {
                const auto program = material->handle();

                // create default uniforms
                uniforms_[material][render_entity] = std::make_unique<DefaultUniforms>(
                    OpenGLUniform(program, "projection"),
                    OpenGLUniform(program, "view"),
                    OpenGLUniform(program, "model"),
                    OpenGLUniform(program, "normal_matrix", false),
                    OpenGLUniform(program, "light_colour", false),
                    OpenGLUniform(program, "light_position", false),
                    OpenGLUniform(program, "light_attenuation", false),
                    OpenGLUniform(program, "g_shadow_map", false),
                    OpenGLUniform(program, "light_projection", false),
                    OpenGLUniform(program, "light_view", false),
                    OpenGLUniform(program, "bones"));

                // create uniforms for each texture
                for (auto i = 0u; i < material->textures().size(); ++i)
                {
                    uniforms_[material][render_entity]->textures.emplace_back(
                        OpenGLUniform{program, "texture" + std::to_string(i), false});
                }
            }
        }
    }
}

RenderTarget *OpenGLRenderer::create_render_target(std::uint32_t width, std::uint32_t height)
{
    auto &tex_man = static_cast<OpenGLTextureManager &>(Root::texture_manager());

    const auto scale = Root::window_manager().current_window()->screen_scale();

    render_targets_.emplace_back(std::make_unique<OpenGLRenderTarget>(
        std::make_unique<OpenGLTexture>(
            DataBuffer{}, width * scale, height * scale, TextureUsage::RENDER_TARGET, tex_man.next_id()),
        std::make_unique<OpenGLTexture>(
            DataBuffer{}, width * scale, height * scale, TextureUsage::DEPTH, tex_man.next_id())));

    return render_targets_.back().get();
}

void OpenGLRenderer::execute_pass_start(RenderCommand &command)
{
    const auto *target = static_cast<const OpenGLRenderTarget *>(command.render_pass()->render_target);

    // if we have no target then we render to the default framebuffer
    // else we bind the supplied target
    if (target == nullptr)
    {
        const auto scale = Root::window_manager().current_window()->screen_scale();

        ::glViewport(0, 0, width_ * scale, height_ * scale);
        expect(check_opengl_error, "could not set viewport");

        ::glBindFramebuffer(GL_FRAMEBUFFER, 0);
        expect(check_opengl_error, "could not bind default buffer");
    }
    else
    {
        ::glViewport(0, 0, target->colour_texture()->width(), target->colour_texture()->height());
        expect(check_opengl_error, "could not set viewport");

        target->bind(GL_FRAMEBUFFER);
    }

    // clear current target
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    expect(check_opengl_error, "could not clear");
}

void OpenGLRenderer::execute_draw(RenderCommand &command)
{
    const auto *camera = command.render_pass()->camera;
    const auto *render_entity = command.render_entity();
    const auto *light = command.light();

    static const OpenGLRenderTarget *previous_target = nullptr;
    const auto *target = static_cast<const OpenGLRenderTarget *>(command.render_pass()->render_target);

    // optimisation, we only call render_setup when the target changes
    if (target != previous_target)
    {
        render_setup(target);
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

    const auto *uniforms = uniforms_[material][render_entity].get();

    set_uniforms(uniforms, camera, render_entity, command.shadow_map(), light);
    bind_textures(uniforms, material);
    draw_meshes(render_entity);
}

void OpenGLRenderer::execute_present(RenderCommand &)
{
#if defined(IRIS_PLATFORM_MACOS)
    ::glSwapAPPLE();
#elif defined(IRIS_PLATFORM_WIN32)
    const auto *window = static_cast<Win32OpenGLWindow *>(Root::window_manager().current_window());
    ::SwapBuffers(window->device_context());
#endif
}

}
