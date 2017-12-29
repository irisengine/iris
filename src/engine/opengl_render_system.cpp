#include "opengl_render_system.hpp"

#include <string>
#include <utility>

#include "entity.hpp"
#include "gl/auto_bind.hpp"
#include "gl/buffer.hpp"
#include "gl/material.hpp"
#include "gl/opengl.hpp"
#include "gl/shader.hpp"
#include "gl/shader_type.hpp"
#include "gl/vertex_state.hpp"
#include "mesh.hpp"
#include "vector3.hpp"

namespace
{
    // hard coded shaders

    static const std::string vertex_source { R"(
    #version 330 core
    precision mediump float;
    layout (location = 0) in vec3 position;
    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;
    void main()
    {
        gl_Position = transpose(projection) * transpose(view) *transpose(model) * vec4(position, 1.0);
    }
)" };

    static const std::string fragment_source { R"(
    #version 330 core
    precision mediump float;
    out vec4 outColor;
    uniform vec3 colour;
    void main()
    {
        outColor = vec4(colour, 1.0);
    }
)"};

}

namespace eng
{

opengl_render_system::opengl_render_system(
            std::shared_ptr<camera> c,
            const float width,
            const float height)
    : scene_(),
      camera_(c),
      material_(nullptr)
{
    ::glViewport(
        0,
        0,
        static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height));
    gl::check_opengl_error("could not set viewport");

    // create the material to render entities with
    const auto vertex_shader = gl::shader(vertex_source, gl::shader_type::VERTEX);
    const auto fragment_shader = gl::shader(fragment_source, gl::shader_type::FRAGMENT);
    material_ = std::make_unique<gl::material>(vertex_shader, fragment_shader);
}

void opengl_render_system::add(std::shared_ptr<entity> e)
{
    scene_.emplace_back(e);
}

void opengl_render_system::render() const
{
    // render each element in scene
    for(const auto &e : scene_)
    {
        gl::auto_bind<gl::material> auto_program{ *material_ };

        const auto proj_uniform = ::glGetUniformLocation(material_->native_handle(), "projection");
        gl::check_opengl_error("could not get projection uniform location");

        ::glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, camera_->projection().data());
        gl::check_opengl_error("could not set projection matrix uniform data");

        const auto view_uniform = ::glGetUniformLocation(material_->native_handle(), "view");
        gl::check_opengl_error("could not get view uniform location");

        ::glUniformMatrix4fv(view_uniform, 1, GL_FALSE, camera_->view().data());
        gl::check_opengl_error("could not set view matrix uniform data");

        const auto model_uniform = ::glGetUniformLocation(material_->native_handle(), "model");
        gl::check_opengl_error("could not get model uniform location");

        // render each mesh in element
        for(const auto &m : e->meshes())
        {
            gl::auto_bind<mesh> auto_mesh{ m };

            ::glUniformMatrix4fv(model_uniform, 1, GL_FALSE, m.model().data());
            gl::check_opengl_error("could not set model matrix uniform data");

            const auto colour_uniform = ::glGetUniformLocation(material_->native_handle(), "colour");
            gl::check_opengl_error("could not get colour uniform location");

            const auto r = static_cast<float>((m.colour() >> 24) & 0xFF) / 255.0f;
            const auto g = static_cast<float>((m.colour() >> 16) & 0xFF) / 255.0f;
            const auto b = static_cast<float>((m.colour() >>  8) & 0xFF) / 255.0f;

            ::glUniform3f(colour_uniform, r, g, b);
            gl::check_opengl_error("could not set colour uniform data");

            // draw!
            ::glDrawElements(GL_TRIANGLES, m.indices().size(), GL_UNSIGNED_INT, 0);
            gl::check_opengl_error("could not draw triangles");
        }
    }
}

}

