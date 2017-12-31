#include "opengl_render_system.hpp"

#include <string>
#include <utility>

#include "auto_bind.hpp"
#include "entity.hpp"
#include "gl/buffer.hpp"
#include "gl/opengl.hpp"
#include "gl/vertex_state.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "vector3.hpp"

namespace
{
    // hard coded shaders

    static const std::string vertex_source { R"(
    #version 330 core
    precision mediump float;
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec3 tex;
    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;
    out vec2 texCoord;
    void main()
    {
        gl_Position = transpose(projection) * transpose(view) *transpose(model) * vec4(position, 1.0);
        texCoord = vec2(tex.x, tex.y);
    }
)" };

    static const std::string fragment_source { R"(
    #version 330 core
    precision mediump float;
    in vec2 texCoord;
    out vec4 outColor;
    uniform vec3 colour;
    uniform sampler2D texture1;
    void main()
    {
        outColor = texture(texture1, texCoord);
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
      material_(vertex_source, fragment_source)
{
    ::glEnable(GL_DEPTH_TEST);
    gl::check_opengl_error("could not enable depth testing");

    ::glDepthFunc(GL_LESS);
    gl::check_opengl_error("could not set depth test function");

    ::glViewport(
        0,
        0,
        static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height));
    gl::check_opengl_error("could not set viewport");
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
        auto_bind<material> auto_program{ material_ };

        const auto program = material_.native_handle<std::uint32_t>();

        const auto proj_uniform = ::glGetUniformLocation(program, "projection");
        gl::check_opengl_error("could not get projection uniform location");

        ::glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, camera_->projection().data());
        gl::check_opengl_error("could not set projection matrix uniform data");

        const auto view_uniform = ::glGetUniformLocation(program, "view");
        gl::check_opengl_error("could not get view uniform location");

        ::glUniformMatrix4fv(view_uniform, 1, GL_FALSE, camera_->view().data());
        gl::check_opengl_error("could not set view matrix uniform data");

        const auto model_uniform = ::glGetUniformLocation(program, "model");
        gl::check_opengl_error("could not get model uniform location");

        // render each mesh in element
        for(const auto &m : e->meshes())
        {
            auto_bind<mesh> auto_mesh{ m };

            ::glUniformMatrix4fv(model_uniform, 1, GL_FALSE, m.model().data());
            gl::check_opengl_error("could not set model matrix uniform data");

            const auto colour_uniform = ::glGetUniformLocation(program, "colour");
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

void opengl_render_system::set_wireframe_mode(bool wireframe)
{
    const auto mode = wireframe
        ? GL_LINE
        : GL_FILL;
    ::glPolygonMode(GL_FRONT_AND_BACK, mode);
}

}

