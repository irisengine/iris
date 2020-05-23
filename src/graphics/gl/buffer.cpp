#include "graphics/buffer.h"

#include <any>
#include <cstdint>
#include <utility>
#include <vector>

#include "core/exception.h"
#include "graphics/buffer_type.h"
#include "graphics/gl/opengl.h"
#include "graphics/vertex_data.h"

namespace
{

/**
 * Helper function to convert internal Buffer type to a opengl type.
 *
 * @param type
 *   Type to convert.
 *
 * @returns
 *   Supplied type as opengl type.
 */
std::uint32_t type_to_gl_type(const eng::BufferType type)
{
    auto gl_type = GL_ARRAY_BUFFER;

    switch(type)
    {
        case eng::BufferType::VERTEX_ATTRIBUTES:
            gl_type = GL_ARRAY_BUFFER;
            break;
        case eng::BufferType::VERTEX_INDICES:
            gl_type = GL_ELEMENT_ARRAY_BUFFER;
            break;
        default:
            throw eng::Exception("unknown Buffer type");
    }

    return gl_type;
}

/**
 * Helper function to create a opengl Buffer from a collection of objects.
 *
 * @param data
 *   Data to store on buffer.
 *
 * @returns
 *   Handle to opengl buffer.
 */
template<class T>
std::uint32_t create_Buffer(
    const std::vector<T> &data,
    const eng::BufferType type)
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
    eng::check_opengl_error("could not Buffer data");

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
struct Buffer::implementation final
{
    /** Simple constructor which takes a value for each member. */
    implementation(std::uint32_t handle)
        : handle(handle)
    { }

    /** Opengl handle for buffer. */
    std::uint32_t handle;
};

Buffer::Buffer(const std::vector<float> &data, const BufferType type)
    : impl_(std::make_unique<implementation>(create_Buffer(data, type))),
      type_(type)
{ }

Buffer::Buffer(const std::vector<std::uint32_t> &data, const BufferType type)
    : impl_(std::make_unique<implementation>(create_Buffer(data, type))),
      type_(type)
{ }

Buffer::Buffer(const std::vector<vertex_data> &data, const BufferType type)
    : impl_(std::make_unique<implementation>(create_Buffer(data, type))),
      type_(type)
{ }

Buffer::~Buffer()
{
    if(impl_)
    {
        ::glDeleteBuffers(1, std::addressof(impl_->handle));
    }
}

/** Default. */
Buffer::Buffer(Buffer &&other) = default;
Buffer& Buffer::operator=(Buffer &&other) = default;

std::any Buffer::native_handle() const
{
    return std::any{ impl_->handle };
}

BufferType Buffer::type() const
{
    return type_;
}

}
