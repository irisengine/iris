#include "graphics/render_system.hpp"

#include <string>
#include <utility>

#include "core/vector3.hpp"
#include "graphics/buffer.hpp"
#include "graphics/gl/opengl.hpp"
#include "graphics/material.hpp"
#include "graphics/mesh.hpp"
#include "graphics/sprite.hpp"
#include "log/log.hpp"

namespace eng
{

/**
 * Struct contatining implementation specific data.
 */
struct render_system::implementation
{
};

render_system::render_system()
    : scene_(),
      camera_(),
      light_position(),
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
render_system::~render_system() = default;
render_system::render_system(render_system&&) = default;
render_system& render_system::operator=(render_system&&) = default;

void render_system::render() const
{
    // clear current target
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render each element in scene
    for(const auto &e : scene_)
    {
        // bind material to render with
        const auto program = std::any_cast<std::uint32_t>(e->mat().native_handle());
        ::glUseProgram(program);
        check_opengl_error("could not bind program");

        if(e->should_render_wireframe())
        {
            ::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        // set uniforms

        const auto proj_uniform = ::glGetUniformLocation(program, "projection");
        check_opengl_error("could not get projection uniform location");

        ::glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, camera_.projection().data());
        check_opengl_error("could not set projection matrix uniform data");

        const auto view_uniform = ::glGetUniformLocation(program, "view");
        check_opengl_error("could not get view uniform location");

        ::glUniformMatrix4fv(view_uniform, 1, GL_FALSE, camera_.view().data());
        check_opengl_error("could not set view matrix uniform data");

        const auto light_uniform = ::glGetUniformLocation(program, "light");
        check_opengl_error("could not get light uniform location");

        ::glUniform3f(light_uniform, light_position.x, light_position.y, light_position.z);
        check_opengl_error("could not set light uniform data");

        const auto model_uniform = ::glGetUniformLocation(program, "model");
        check_opengl_error("could not get model uniform location");

        ::glUniformMatrix4fv(model_uniform, 1, GL_FALSE, e->transform().data());
        check_opengl_error("could not set model matrix uniform data");


        // bind mesh so the final draw call renders it
        const auto vao = std::any_cast<std::uint32_t>(e->render_mesh().native_handle());

        // bind the vao
        ::glBindVertexArray(vao);
        check_opengl_error("could not bind vao");

        const auto tex_handle = std::any_cast<std::uint32_t>(e->render_mesh().tex().native_handle());
        // use default texture unit
        ::glActiveTexture(GL_TEXTURE0);
        check_opengl_error("could not activiate texture");

        ::glBindTexture(GL_TEXTURE_2D, tex_handle);
        check_opengl_error("could not bind texture");

        // draw!
        ::glDrawElements(GL_TRIANGLES, e->render_mesh().indices().size(), GL_UNSIGNED_INT, 0);
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
}

sprite* render_system::add(std::unique_ptr<sprite> s)
{
    scene_.emplace_back(std::move(s));
    return scene_.back().get();
}

void render_system::set_light_position(const vector3 &position)
{
    light_position = position;
    LOG_ENGINE_INFO("render_system", "light position set: {}", light_position);
}

}

