#include "graphics/buffer_descriptor.h"

#include <memory>

#include "core/exception.h"
#include "graphics/gl/opengl.h"
#include "graphics/vertex_attributes.h"

namespace
{
GLenum to_opengl_format(iris::VertexAttributeType type)
{
    GLenum format = 0u;

    switch (type)
    {
        case iris::VertexAttributeType::FLOAT_3:
            format = GL_FLOAT;
            break;
        case iris::VertexAttributeType::FLOAT_4:
            format = GL_FLOAT;
            break;
        case iris::VertexAttributeType::UINT32_1:
            format = GL_UNSIGNED_INT;
            break;
        default:
            throw iris::Exception("unknown vertex attribute type");
    }

    return format;
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
    const auto &attr = attributes.attributes();

    // ensure both buffers are bound for the vao
    bind_buffer(vertex_buffer_);
    bind_buffer(index_buffer_);

    for (auto i = 0u; i < attr.size(); ++i)
    {
        const auto &[type, components, _, offset] = attr[i];

        ::glEnableVertexAttribArray(i);
        check_opengl_error("could not enable attribute");

        ::glVertexAttribPointer(
            i,
            components,
            to_opengl_format(type),
            GL_FALSE,
            attributes.size(),
            reinterpret_cast<void *>(offset));
        std::cout << components << " " << offset << std::endl;
        check_opengl_error("could not set attributes");
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
