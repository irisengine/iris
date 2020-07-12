#include "graphics/render_system.h"

#include <string>
#include <utility>

#include "core/camera.h"
#include "core/camera_type.h"
#include "core/vector3.h"
#include "graphics/buffer.h"
#include "graphics/gl/opengl.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/render_entity.h"
#include "log/log.h"

namespace eng
{

/**
 * Struct contatining implementation specific data.
 */
struct RenderSystem::implementation
{
};

RenderSystem::RenderSystem(float width, float height)
    : scene_(),
      persective_camera_(CameraType::PERSPECTIVE, width, height),
      orthographic_camera_(CameraType::ORTHOGRAPHIC, width, height),
      impl_(nullptr)
{
    // opengl setup

    ::glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    check_opengl_error("could not set clear colour");

    ::glEnable(GL_DEPTH_TEST);
    check_opengl_error("could not enable depth testing");

    ::glDepthFunc(GL_LESS);
    check_opengl_error("could not set depth test function");

    LOG_ENGINE_INFO("render_system", "constructed opengl render system");
}

/** Default */
RenderSystem::~RenderSystem() = default;
RenderSystem::RenderSystem(RenderSystem&&) = default;
RenderSystem& RenderSystem::operator=(RenderSystem&&) = default;

void RenderSystem::render()
{
    // clear current target
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render each element in scene
    for(const auto &e : scene_)
    {
        // bind Material to render with
        const auto program = std::any_cast<std::uint32_t>(e->material().native_handle());
        ::glUseProgram(program);
        check_opengl_error("could not bind program");

        if(e->should_render_wireframe())
        {
            ::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        // set uniforms

        const auto proj_uniform = ::glGetUniformLocation(program, "projection");
        check_opengl_error("could not get projection uniform location");

        auto &cam = camera(e->camera_type());

        ::glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, reinterpret_cast<const float*>(cam.projection().data()));
        check_opengl_error("could not set projection matrix uniform data");

        const auto view_uniform = ::glGetUniformLocation(program, "view");
        check_opengl_error("could not get view uniform location");

        ::glUniformMatrix4fv(view_uniform, 1, GL_FALSE, reinterpret_cast<const float*>(cam.view().data()));
        check_opengl_error("could not set view matrix uniform data");

        const auto model_uniform = ::glGetUniformLocation(program, "model");
        check_opengl_error("could not get model uniform location");

        ::glUniformMatrix4fv(model_uniform, 1, GL_FALSE, reinterpret_cast<const float*>(e->transform().data()));
        check_opengl_error("could not set model matrix uniform data");

        const auto normal_uniform = ::glGetUniformLocation(program, "normal_matrix");
        check_opengl_error("could not get normal uniform location");

        ::glUniformMatrix4fv(normal_uniform, 1, GL_FALSE, reinterpret_cast<const float*>(e->normal_transform().data()));
        check_opengl_error("could not set normal matrix uniform data");

        const auto light_uniform = ::glGetUniformLocation(program, "light");
        check_opengl_error("could not get light uniform location");

        static auto light = eng::Vector3(100.0f, 100.0f, 100.0f);
        ::glUniform3f(light_uniform, light.x, light.y, light.z);
        check_opengl_error("could not set light uniform data");

        // bind Mesh so the final draw call renders it
        const auto vao = std::any_cast<std::uint32_t>(e->mesh().native_handle());

        // bind the vao
        ::glBindVertexArray(vao);
        check_opengl_error("could not bind vao");

        const auto tex_handle = std::any_cast<std::uint32_t>(e->mesh().texture().native_handle());
        // use default Texture unit
        ::glActiveTexture(GL_TEXTURE0);
        check_opengl_error("could not activiate texture");

        ::glBindTexture(GL_TEXTURE_2D, tex_handle);
        check_opengl_error("could not bind texture");

        // draw!
        ::glDrawElements(GL_TRIANGLES, e->mesh().indices().size(), GL_UNSIGNED_INT, 0);
        check_opengl_error("could not draw triangles");

        // unbind vao
        ::glBindVertexArray(0u);
        check_opengl_error("could not unbind vao");

        if(e->should_render_wireframe())
        {
            ::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // unbind program
        ::glUseProgram(0u);
        check_opengl_error("could not unbind program");
    }

#if defined(IRIS_GRAPHICS_API_OPENGL) and defined(PLATFORM_MACOS)
    ::glSwapAPPLE();
#endif
}

RenderEntity* RenderSystem::add(std::unique_ptr<RenderEntity> entity)
{
    scene_.emplace_back(std::move(entity));
    return scene_.back().get();
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

