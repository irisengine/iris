#include "gl/vertex_data.hpp"

#include <cstdint>
#include <utility>
#include <vector>

#include "gl/opengl.hpp"

namespace eng::gl
{

vertex_data::~vertex_data()
{
    ::glDeleteBuffers(1, &vbo_);
}

vertex_data::vertex_data(vertex_data &&other) noexcept
    : vbo_(0u)
{
    std::swap(vbo_, other.vbo_);
}

vertex_data& vertex_data::operator=(vertex_data &&other) noexcept
{
    // create a new data object to 'steal' the internal state of the supplied
    // object then swap
    // this ensures that the current data is correctly deleted at the end
    // of this call
    vertex_data new_data(std::move(other));
    std::swap(vbo_, new_data.vbo_);

    return *this;
}

void vertex_data::bind() const
{
    ::glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    gl::check_opengl_error("could not bind vbo");
}

void vertex_data::unbind() const
{
    ::glBindBuffer(GL_ARRAY_BUFFER, 0u);
    gl::check_opengl_error("could not unbind vbo");
}

}

