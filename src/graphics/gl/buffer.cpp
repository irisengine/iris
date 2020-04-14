#include "graphics/buffer.hpp"

#include <any>
#include <cstdint>
#include <utility>
#include <vector>

#include "core/exception.hpp"
#include "graphics/buffer_type.hpp"
#include "graphics/gl/opengl.hpp"
#include "graphics/vertex_data.hpp"

namespace
{

/**
 * Helper function to convert internal buffer type to a opengl type.
 *
 * @param type
 *   Type to convert.
 *
 * @returns
 *   Supplied type as opengl type.
 */
std::uint32_t type_to_gl_type(const eng::buffer_type type)
{
    auto gl_type = GL_ARRAY_BUFFER;

    switch(type)
    {
        case eng::buffer_type::VERTEX_ATTRIBUTES:
            gl_type = GL_ARRAY_BUFFER;
            break;
        case eng::buffer_type::VERTEX_INDICES:
            gl_type = GL_ELEMENT_ARRAY_BUFFER;
            break;
        default:
            throw eng::exception("unknown buffer type");
    }

    return gl_type;
}

/**
 * Helper function to create a opengl buffer from a collection of objects.
 *
 * @param data
 *   Data to store on buffer.
 *
 * @returns
 *   Handle to opengl buffer.
 */
template<class T>
std::uint32_t create_buffer(
    const std::vector<T> &data,
    const eng::buffer_type type)
{
    std::uint32_t handle = 0u;

    ::glGenBuffers(1, &handle);
    eng::check_opengl_error("could not generate opengl buffer");

    const auto gl_type = type_to_gl_type(type);

    // bind so we can copy data
    ::glBindBuffer(gl_type, handle);
    eng::check_opengl_error("could not bind buffer");

    // copy data to buffer
    ::glBufferData(
        gl_type,
        data.size() * sizeof(T),
        data.data(),
        GL_STATIC_DRAW);
    eng::check_opengl_error("could not buffer data");

    // unbind buffer
    ::glBindBuffer(gl_type, 0u);
    eng::check_opengl_error("could not unbind buffer");

    return handle;
}

}

namespace eng
{

/**
 * Struct containing implementation specific data.
 */
struct buffer::implementation final
{
    /** Simple constructor which takes a value for each member. */
    implementation(std::uint32_t handle)
        : handle(handle)
    { }

    /** Opengl handle for buffer. */
    std::uint32_t handle;
};

buffer::buffer(const std::vector<float> &data, const buffer_type type)
    : impl_(std::make_unique<implementation>(create_buffer(data, type))),
      type_(type)
{ }

buffer::buffer(const std::vector<std::uint32_t> &data, const buffer_type type)
    : impl_(std::make_unique<implementation>(create_buffer(data, type))),
      type_(type)
{ }

buffer::buffer(const std::vector<vertex_data> &data, const buffer_type type)
    : impl_(std::make_unique<implementation>(create_buffer(data, type))),
      type_(type)
{ }

buffer::~buffer()
{
    if(impl_)
    {
        ::glDeleteBuffers(1, std::addressof(impl_->handle));
    }
}

/** Default. */
buffer::buffer(buffer &&other) = default;
buffer& buffer::operator=(buffer &&other) = default;

std::any buffer::native_handle() const
{
    return std::any{ impl_->handle };
}

buffer_type buffer::type() const
{
    return type_;
}

}

