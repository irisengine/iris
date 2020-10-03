#include "graphics/buffer_descriptor.h"

#include <memory>
#include <tuple>

#include "core/exception.h"
#include "graphics/gl/opengl.h"
#include "graphics/vertex_attributes.h"

namespace
{

std::tuple<GLenum, bool> to_opengl_format(iris::VertexAttributeType type)
{
    GLenum format = 0u;
    auto is_float = true;

    switch (type)
    {
        case iris::VertexAttributeType::FLOAT_3:
        case iris::VertexAttributeType::FLOAT_4:
            format = GL_FLOAT;
            break;
        case iris::VertexAttributeType::UINT32_1:
        case iris::VertexAttributeType::UINT32_4:
            format = GL_UNSIGNED_INT;
            is_float = false;
            break;
        default:
            throw iris::Exception("unknown vertex attribute type");
    }

    return {format, is_float};
}

/**
 * Helper function to convert internal Buffer type to a opengl type.
 *
 * @param type
 *   Type to convert.
 *
 * @returns
 *   Supplied type as opengl type.
 */
std::uint32_t type_to_gl_type(const iris::BufferType type)
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
        default:
            throw iris::Exception("unknown Buffer type");
    }

    return gl_type;
}

/**
 * Helper function to bind an opengl buffer.
 *
 * @param buffer
 *   Buffer to bind.
 */
void bind_buffer(const iris::Buffer &buffer)
{
    const auto handle = std::any_cast<std::uint32_t>(buffer.native_handle());

    ::glBindBuffer(type_to_gl_type(buffer.type()), handle);
    iris::check_opengl_error("could not bind buffer");
}

}

namespace iris
{

struct BufferDescriptor::implementation
{
    std::uint32_t vao;
};

BufferDescriptor::BufferDescriptor(
    Buffer vertex_buffer,
    Buffer index_buffer,
    const VertexAttributes &attributes)
    : vertex_buffer_(std::move(vertex_buffer))
    , index_buffer_(std::move(index_buffer))
    , impl_(std::make_unique<implementation>())
{
    // create vao
    ::glGenVertexArrays(1, std::addressof(impl_->vao));
    check_opengl_error("could not generate vao");

    // bind the vao
    ::glBindVertexArray(impl_->vao);
    check_opengl_error("could not bind vao");

    // ensure both buffers are bound for the vao
    bind_buffer(vertex_buffer_);
    bind_buffer(index_buffer_);

    auto index = 0u;

    for (const auto &attribute : attributes)
    {
        const auto &[type, components, _, offset] = attribute;

        ::glEnableVertexAttribArray(index);
        check_opengl_error("could not enable attribute");

        const auto &[open_gl_type, is_float] = to_opengl_format(type);

        if (is_float)
        {
            ::glVertexAttribPointer(
                index,
                components,
                open_gl_type,
                GL_FALSE,
                attributes.size(),
                reinterpret_cast<void *>(offset));
            check_opengl_error("could not set attributes");
        }
        else
        {
            ::glVertexAttribIPointer(
                index,
                components,
                open_gl_type,
                attributes.size(),
                reinterpret_cast<void *>(offset));
            check_opengl_error("could not set attributes");
        }

        ++index;
    }

    // unbind vao
    ::glBindVertexArray(0u);
    check_opengl_error("could not unbind vao");
}

BufferDescriptor::~BufferDescriptor() = default;
BufferDescriptor::BufferDescriptor(BufferDescriptor &&) = default;
BufferDescriptor &BufferDescriptor::operator=(BufferDescriptor &&) = default;

const Buffer &BufferDescriptor::vertex_buffer() const
{
    return vertex_buffer_;
}

const Buffer &BufferDescriptor::index_buffer() const
{
    return index_buffer_;
}

std::any BufferDescriptor::native_handle() const
{
    return {impl_->vao};
}

}
