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

namespace
{
    // hard coded shaders

    static const std::string vertex_source { R"(
    #version 330 core
    precision mediump float;
    in vec2 position;
    void main()
    {
        gl_Position = vec4(position, 0.0, 1.0);
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

        // set uniform
        const auto uniform = ::glGetUniformLocation(material.native_handle(), "colour");
        gl::check_opengl_error("could not get colour uniform location");

        const auto r = static_cast<float>((mesh->colour() >> 24) & 0xFF) / 255.0f;
        const auto g = static_cast<float>((mesh->colour() >> 16) & 0xFF) / 255.0f;
        const auto b = static_cast<float>((mesh->colour() >>  8) & 0xFF) / 255.0f;

        ::glUniform3f(uniform, r, g, b);
        gl::check_opengl_error("could not set uniform data");

        // draw!
        ::glDrawArrays(GL_TRIANGLES, 0, 3);
        gl::check_opengl_error("could not draw triangles");
    }
}

}

