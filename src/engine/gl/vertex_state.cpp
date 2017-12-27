#include "gl/vertex_state.hpp"

#include <cstdint>
#include <utility>

#include "gl/opengl.hpp"

namespace eng::gl
{

vertex_state::vertex_state()
    : vao_(0u)
{
    ::glGenVertexArrays(1, &vao_);
    gl::check_opengl_error("could not generate vao");
}

vertex_state::~vertex_state()
{
    ::glDeleteVertexArrays(1, &vao_);
}

vertex_state::vertex_state(vertex_state &&other) noexcept
    : vao_(0u)
{
    std::swap(vao_, other.vao_);
}

vertex_state& vertex_state::operator=(vertex_state &&other) noexcept
{
    // create a new state object to 'steal' the internal state of the supplied
    // object then swap
    // this ensures that the current state is correctly deleted at the end
    // of this call
    vertex_state new_state(std::move(other));
    std::swap(vao_, new_state.vao_);

    return *this;
}

std::uint32_t vertex_state::native_handle() const noexcept
{
    return vao_;
}

void vertex_state::bind() const
{
    ::glBindVertexArray(vao_);
    gl::check_opengl_error("could not bind vao");
}

void vertex_state::unbind() const
{
    ::glBindVertexArray(0u);
    gl::check_opengl_error("could not unbind vao");
}

}

