#include "gl/buffer.hpp"

#include <cstdint>
#include <utility>
#include <vector>

#include "gl/opengl.hpp"

namespace eng::gl
{

buffer::~buffer()
{
    ::glDeleteBuffers(1, &handle_);
}

buffer::buffer(buffer &&other) noexcept
    : handle_(0u)
{
    std::swap(handle_, other.handle_);
}

buffer& buffer::operator=(buffer &&other) noexcept
{
    // create a new data object to 'steal' the internal state of the supplied
    // object then swap
    // this ensures that the current data is correctly deleted at the end
    // of this call
    buffer new_data(std::move(other));
    std::swap(handle_, new_data.handle_);

    return *this;
}

void buffer::bind() const
{
    ::glBindBuffer(type_, handle_);
    gl::check_opengl_error("could not bind buffer");
}

void buffer::unbind() const
{
    ::glBindBuffer(type_, 0u);
    gl::check_opengl_error("could not unbind buffer");
}

}

