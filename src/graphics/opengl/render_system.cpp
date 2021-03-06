#include "graphics/render_system.h"

#include <string>
#include <utility>

#include "core/camera.h"
#include "core/vector3.h"
#include "graphics/buffer.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/material.h"
#include "graphics/opengl/opengl.h"
#include "graphics/pipeline.h"
#include "graphics/render_entity.h"
#include "log/log.h"

namespace
{

/**
 * Helper function to setup opengl for a render pass.
 *
 * @param target
 *   RenderTarget for render pass.
 */
void render_setup(const iris::RenderTarget *target)
{
    ::glViewport(
        0,
        0,
        target->colour_texture()->width(),
        target->colour_texture()->height());
    iris::check_opengl_error("could not set viewport");

    ::glBindFramebuffer(
        GL_FRAMEBUFFER, std::any_cast<GLuint>(target->native_handle()));
    iris::check_opengl_error("could not bind custom fbo");

    // clear current target
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * Helper function to set uniforms for a render pass.
 *
 * @param program
 *   Opengl program handle to set uniforms for.
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
    GLuint program,
    const iris::Camera &camera,
    iris::RenderEntity *entity,
    const iris::LightingRig *lighting_rig)
{
    const auto proj_uniform = ::glGetUniformLocation(program, "projection");
    iris::check_opengl_error("could not get projection uniform location");

    ::glUniformMatrix4fv(
        proj_uniform,
        1,
        GL_TRUE,
        reinterpret_cast<const float *>(camera.projection().data()));
    iris::check_opengl_error("could not set projection matrix uniform data");

    const auto view_uniform = ::glGetUniformLocation(program, "view");
    iris::check_opengl_error("could not get view uniform location");

    ::glUniformMatrix4fv(
        view_uniform,
        1,
        GL_TRUE,
        reinterpret_cast<const float *>(camera.view().data()));
    iris::check_opengl_error("could not set view matrix uniform data");

    const auto model_uniform = ::glGetUniformLocation(program, "model");
    iris::check_opengl_error("could not get model uniform location");

    ::glUniformMatrix4fv(
        model_uniform,
        1,
        GL_TRUE,
        reinterpret_cast<const float *>(entity->transform().data()));
    iris::check_opengl_error("could not set model matrix uniform data");

    const auto normal_uniform =
        ::glGetUniformLocation(program, "normal_matrix");
    iris::check_opengl_error("could not get normal uniform location");

    ::glUniformMatrix4fv(
        normal_uniform,
        1,
        GL_TRUE,
        reinterpret_cast<const float *>(entity->normal_transform().data()));
    iris::check_opengl_error("could not set normal matrix uniform data");

    for (auto i = 0u; i < lighting_rig->directional_lights.size(); ++i)
    {
        const auto light = lighting_rig->directional_lights[i].get();

        const auto light_name = std::string{"light"} + std::to_string(i);
        const auto light_uniform =
            ::glGetUniformLocation(program, light_name.c_str());
        iris::check_opengl_error("could not get light uniform location");

        const auto light_dir = light->direction();

        ::glUniform3f(light_uniform, light_dir.x, light_dir.y, light_dir.z);
        iris::check_opengl_error("could not set light uniform data");

        const auto light_proj_name =
            std::string{"light_proj"} + std::to_string(i);
        const auto light_proj_uniform =
            ::glGetUniformLocation(program, light_proj_name.c_str());
        iris::check_opengl_error("could not get light proj uniform location");

        const auto light_proj = light->shadow_camera().projection();

        ::glUniformMatrix4fv(
            light_proj_uniform,
            1,
            GL_TRUE,
            reinterpret_cast<const float *>(light_proj.data()));
        iris::check_opengl_error(
            "could not set projection matrix uniform data");

        const auto light_view_name =
            std::string{"light_view"} + std::to_string(i);
        const auto light_view_uniform =
            ::glGetUniformLocation(program, light_view_name.c_str());
        iris::check_opengl_error("could not get light view uniform location");

        const auto light_view = light->shadow_camera().view();

        ::glUniformMatrix4fv(
            light_view_uniform,
            1,
            GL_TRUE,
            reinterpret_cast<const float *>(light_view.data()));
        iris::check_opengl_error(
            "could not set projection matrix uniform data");
    }

    for (auto i = 0u; i < lighting_rig->point_lights.size(); ++i)
    {
        const auto light = lighting_rig->point_lights[i].get();

        const auto light_name =
            std::string{"light"} +
            std::to_string(i + lighting_rig->directional_lights.size());
        const auto light_uniform =
            ::glGetUniformLocation(program, light_name.c_str());
        iris::check_opengl_error("could not get light uniform location");

        const auto light_pos = light->position();

        ::glUniform3f(light_uniform, light_pos.x, light_pos.y, light_pos.z);
        iris::check_opengl_error("could not set light uniform data");
    }

    const auto bones_uniform = ::glGetUniformLocation(program, "bones");
    iris::check_opengl_error("could not get bones uniform location");

    ::glUniformMatrix4fv(
        bones_uniform,
        static_cast<GLsizei>(entity->skeleton().transforms().size()),
        GL_TRUE,
        reinterpret_cast<const float *>(
            entity->skeleton().transforms().data()));
    iris::check_opengl_error("could not set bones uniform data");

    const auto camera_uniform = ::glGetUniformLocation(program, "camera");
    iris::check_opengl_error("could not get camera uniform location");

    const auto camera_position = camera.position();
    ::glUniform3f(
        camera_uniform,
        camera_position.x,
        camera_position.y,
        camera_position.z);
    iris::check_opengl_error("could not set camera uniform data");
}

/**
 * Helper function to bind all textures for a material.
 *
 * @param program
 *   Opengl program handle.
 *
 * @param material
 *   Material to bind textures for.
 */
void bind_textures(GLuint program, const iris::Material *material)
{
    const auto textures = material->textures();
    for (const auto *texture : textures)
    {
        const auto tex_handle = std::any_cast<GLuint>(texture->native_handle());
        const auto id = texture->texture_id();

        // id is texture unit
        ::glActiveTexture(GL_TEXTURE0 + id);
        iris::check_opengl_error("could not activate texture");

        ::glBindTexture(GL_TEXTURE_2D, tex_handle);
        iris::check_opengl_error("could not bind texture");

        std::string uniform{"texture" + std::to_string(id)};
        const auto texture_uniform =
            ::glGetUniformLocation(program, uniform.c_str());
        iris::check_opengl_error("could not get texture uniform location");

        ::glUniform1i(texture_uniform, id);
        iris::check_opengl_error("could not set texture uniform data");
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
    for (const auto &mesh : entity->meshes())
    {
        const auto vao = std::any_cast<GLuint>(mesh.native_handle());

        // bind the vao
        ::glBindVertexArray(vao);
        iris::check_opengl_error("could not bind vao");

        const auto type =
            entity->primitive_type() == iris::PrimitiveType::TRIANGLES
                ? GL_TRIANGLES
                : GL_LINES;

        // draw!
        ::glDrawElements(
            type,
            static_cast<GLsizei>(mesh.index_buffer().element_count()),
            GL_UNSIGNED_INT,
            0);
        iris::check_opengl_error("could not draw triangles");

        // unbind vao
        ::glBindVertexArray(0u);
        iris::check_opengl_error("could not unbind vao");

        if (entity->should_render_wireframe())
        {
            ::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}

/**
 * Helper function to blit a RenderTarget to the screen buffer.
 *
 * @param screen_target
 *   RenderTargtet to blit.
 */
void blit_to_screen(const iris::RenderTarget *screen_target)
{
    const auto *screen_texture = screen_target->colour_texture();

    // bind screen target to "read"
    ::glBindFramebuffer(
        GL_READ_FRAMEBUFFER,
        std::any_cast<GLuint>(screen_target->native_handle()));
    iris::check_opengl_error("could not set read framebuffer");

    // bind default frame buffer to "draw"
    ::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0u);
    iris::check_opengl_error("could not set draw framebuffer");

    // blit from "read" to "draw"
    ::glBlitFramebuffer(
        0,
        0,
        screen_texture->width(),
        screen_texture->height(),
        0,
        0,
        screen_texture->width(),
        screen_texture->height(),
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
        GL_NEAREST);
    iris::check_opengl_error("could not blit to screen");
}

}

namespace iris
{

struct RenderSystem::implementation
{
};

RenderSystem::RenderSystem(
    std::uint32_t,
    std::uint32_t,
    RenderTarget *screen_target)
    : screen_target_(screen_target)
    , impl_(nullptr)
{
    ::glClearColor(0.77f, 0.83f, 0.9f, 1.0f);
    check_opengl_error("could not set clear colour");

    ::glEnable(GL_DEPTH_TEST);
    check_opengl_error("could not enable depth testing");

    ::glDepthFunc(GL_LESS);
    check_opengl_error("could not set depth test function");

    LOG_ENGINE_INFO("render_system", "constructed opengl render system");
}

RenderSystem::~RenderSystem() = default;
RenderSystem::RenderSystem(RenderSystem &&) = default;
RenderSystem &RenderSystem::operator=(RenderSystem &&) = default;

void RenderSystem::render(const Pipeline &pipeline)
{
    for (const auto &stage : pipeline.stages())
    {
        auto &camera = stage->camera();
        auto *target = stage->target();

        // no target means use screen target
        if (target == nullptr)
        {
            target = screen_target_;
        }

        render_setup(target);

        Material *previous_material = nullptr;
        GLuint program = 0u;

        for (const auto &[entity, material, lighting_rig] :
             stage->render_items())
        {
            if (material != previous_material)
            {
                // bind Material to render with
                program = std::any_cast<GLuint>(material->native_handle());
                ::glUseProgram(program);
                check_opengl_error("could not bind program");

                previous_material = material;
            }

            if (entity->should_render_wireframe())
            {
                ::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }

            set_uniforms(program, camera, entity, lighting_rig);
            bind_textures(program, material);
            draw_meshes(entity);
        }
    }

    blit_to_screen(screen_target_);
}

}
