#include "opengl_render_system.hpp"

#include <string>
#include <utility>

#include "gl/auto_bind.hpp"
#include "gl/entity_data.hpp"
#include "gl/material.hpp"
#include "gl/opengl.hpp"
#include "gl/shader.hpp"
#include "gl/shader_type.hpp"
#include "gl/vertex_data.hpp"
#include "gl/vertex_state.hpp"
#include "mesh.hpp"
#include "vector3.hpp"

namespace
{
    // hard coded shaders

    static const std::string vertex_source { R"(
    #version 330 core
    precision mediump float;
    in vec2 position;
    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;
    void main()
    {
        gl_Position = transpose(projection) * transpose(view) *transpose(model) * vec4(position, 0.0, 1.0);
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
      camera_(c)
{
    ::glViewport(
        0,
        0,
        static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height));
    gl::check_opengl_error("could not set viewport");
}

void opengl_render_system::add(std::shared_ptr<mesh> m)
{
    // create a new state and bind
    gl::vertex_state state{ };
    gl::auto_bind<gl::vertex_state> auto_state{ state };

    // create new vertex data and bind
    // we manually bind the data so that it is bound for the scope of the state
    // object
    gl::vertex_data data{ m->vertices() };
    data.bind();

    const auto vertex_shader = gl::shader(vertex_source, gl::shader_type::VERTEX);
    const auto fragment_shader = gl::shader(fragment_source, gl::shader_type::FRAGMENT);

    // create new material and bind
    gl::material material{ vertex_shader, fragment_shader };
    gl::auto_bind<gl::material> auto_program{ material };

    // setup attributes
    const auto pos_attribute = ::glGetAttribLocation(material.native_handle(), "position");
    gl::check_opengl_error("could not get attribute location");

    ::glEnableVertexAttribArray(pos_attribute);
    gl::check_opengl_error("could not enable vertex attribute");

    ::glVertexAttribPointer(pos_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
    gl::check_opengl_error("could not set vertex attributes");

    // add rendering data to scene
    scene_.emplace_back(
        m,
        gl::entity_data{ std::move(state), std::move(data), std::move(material) });
}

void opengl_render_system::render() const
{
    // render each element in scene
    for(const auto &[mesh, data] : scene_)
    {
        auto &&[state, vbo, material] = data;

        // bind state and material
        gl::auto_bind<gl::vertex_state> auto_state{ state };
        gl::auto_bind<gl::material> auto_program{ material };

        const auto proj_uniform = ::glGetUniformLocation(material.native_handle(), "projection");
        gl::check_opengl_error("could not get projection uniform location");

        ::glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, camera_->projection().data());
        gl::check_opengl_error("could not set projection matrix uniform data");

        const auto view_uniform = ::glGetUniformLocation(material.native_handle(), "view");
        gl::check_opengl_error("could not get view uniform location");

        ::glUniformMatrix4fv(view_uniform, 1, GL_FALSE, camera_->view().data());
        gl::check_opengl_error("could not set view matrix uniform data");

        const auto model_uniform = ::glGetUniformLocation(material.native_handle(), "model");
        gl::check_opengl_error("could not get model uniform location");

        ::glUniformMatrix4fv(model_uniform, 1, GL_FALSE, mesh->model().data());
        gl::check_opengl_error("could not set model matrix uniform data");

        const auto colour_uniform = ::glGetUniformLocation(material.native_handle(), "colour");
        gl::check_opengl_error("could not get colour uniform location");

        const auto r = static_cast<float>((mesh->colour() >> 24) & 0xFF) / 255.0f;
        const auto g = static_cast<float>((mesh->colour() >> 16) & 0xFF) / 255.0f;
        const auto b = static_cast<float>((mesh->colour() >>  8) & 0xFF) / 255.0f;

        ::glUniform3f(colour_uniform, r, g, b);
        gl::check_opengl_error("could not set colour uniform data");

        // draw!
        ::glDrawArrays(GL_TRIANGLES, 0, 3);
        gl::check_opengl_error("could not draw triangles");
    }
}

}

