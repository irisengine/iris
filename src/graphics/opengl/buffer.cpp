#include "graphics/buffer.h"

#include <any>
#include <cstdint>
#include <utility>
#include <vector>

#include "core/exception.h"
#include "graphics/buffer_type.h"
#include "graphics/opengl/opengl.h"
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
GLenum type_to_gl_type(const iris::BufferType type)
{
    auto gl_type = GL_ARRAY_BUFFER;

    switch (type)
    {
        case iris::BufferType::VERTEX_ATTRIBUTES:
            gl_type = GL_ARRAY_BUFFER;
            break;
        case iris::BufferType::VERTEX_INDICES:
            gl_type = GL_ELEMENT_ARRAY_BUFFER;
            break;
        default: throw iris::Exception("unknown Buffer type");
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
template <class T>
GLuint create_buffer(const std::vector<T> &data, const iris::BufferType type)
{
    GLuint handle = 0u;

    ::glGenBuffers(1, &handle);
    iris::check_opengl_error("could not generate opengl buffer");

    const auto gl_type = type_to_gl_type(type);

    // bind so we can copy data
    ::glBindBuffer(gl_type, handle);
    iris::check_opengl_error("could not bind buffer");

    // copy data to buffer
    ::glBufferData(
        gl_type, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
    iris::check_opengl_error("could not Buffer data");

    // unbind buffer
    ::glBindBuffer(gl_type, 0u);
    iris::check_opengl_error("could not unbind buffer");

    return handle;
}

}

namespace iris
{

struct Buffer::implementation final
{
    GLuint handle;
};

Buffer::Buffer(
    const DataBuffer &data,
    const BufferType type,
    std::size_t element_count)
    : impl_(std::make_unique<implementation>())
    , type_(type)
    , element_count_(element_count)
    , data_(data)
{
    impl_->handle = create_buffer(data, type);
}

Buffer::~Buffer()
{
    if (impl_)
    {
        ::glDeleteBuffers(1, std::addressof(impl_->handle));
    }
}

Buffer::Buffer(Buffer &&other) = default;
Buffer &Buffer::operator=(Buffer &&other) = default;

std::any Buffer::native_handle() const
{
    return std::any{impl_->handle};
}

BufferType Buffer::type() const
{
    return type_;
}

std::size_t Buffer::element_count() const
{
    return element_count_;
}

const DataBuffer &Buffer::data() const
{
    return data_;
}

}
