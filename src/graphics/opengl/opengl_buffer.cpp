////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/opengl_buffer.h"

#include <cstddef>
#include <cstdint>
#include <vector>

#include "core/error_handling.h"
#include "graphics/opengl/opengl.h"
#include "graphics/vertex_data.h"

namespace
{

/**
 * Helper function to create an opengl buffer from a collection of objects.
 *
 * @param data
 *   Data to store in buffer.
 *
 * @param target
 *   OpenGL buffer binding target.
 *
 * @returns
 *   Handle to opengl buffer.
 */
template <class T>
GLuint create_buffer(const std::vector<T> &data, GLenum target)
{
    GLuint handle = 0u;

    ::glGenBuffers(1, &handle);
    iris::expect(iris::check_opengl_error, "could not generate opengl buffer");

    // bind so we can copy data
    ::glBindBuffer(target, handle);
    iris::expect(iris::check_opengl_error, "could not bind buffer");

    // copy data to buffer
    ::glBufferData(target, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
    iris::expect(iris::check_opengl_error, "could not buffer data");

    // unbind buffer
    ::glBindBuffer(target, 0u);
    iris::expect(iris::check_opengl_error, "could not unbind buffer");

    return handle;
}

/**
 * Helper function to update a buffer with new data.
 *
 * @param data
 *   New data to store in buffer.
 *
 * @param target
 *   OpenGL buffer binding target.
 *
 * @param handle
 *   OpenGL handle to buffer to update.
 *
 * @param element_count
 *   Number of elements currently stored in buffer.
 */
template <class T>
void update_buffer(const std::vector<T> &data, GLenum target, GLuint handle, size_t element_count)
{
    // bind so we can copy data
    ::glBindBuffer(target, handle);
    iris::expect(iris::check_opengl_error, "could not bind buffer");

    if (data.size() > element_count)
    {
        // if we don't have enough space in buffer then create a new data store
        ::glBufferData(target, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
        iris::expect(iris::check_opengl_error, "could not buffer data");
    }
    else
    {
        // if we do have enough space then update existing data store
        ::glBufferSubData(target, 0u, data.size() * sizeof(T), data.data());
        iris::expect(iris::check_opengl_error, "could not sub-buffer data");
    }

    // unbind buffer
    ::glBindBuffer(target, 0u);
    iris::expect(iris::check_opengl_error, "could not unbind buffer");
}

}

namespace iris
{

OpenGLBuffer::OpenGLBuffer(const std::vector<VertexData> &vertex_data)
    : handle_(create_buffer(vertex_data, GL_ARRAY_BUFFER))
    , element_count_(vertex_data.size())
{
}

OpenGLBuffer::OpenGLBuffer(const std::vector<std::uint32_t> &index_data)
    : handle_(create_buffer(index_data, GL_ELEMENT_ARRAY_BUFFER))
    , element_count_(index_data.size())
{
}

OpenGLBuffer::~OpenGLBuffer()
{
    ::glDeleteBuffers(1, &handle_);
}

GLuint OpenGLBuffer::handle() const
{
    return handle_;
}

std::size_t OpenGLBuffer::element_count() const
{
    return element_count_;
}

void OpenGLBuffer::write(const std::vector<VertexData> &vertex_data)
{
    update_buffer(vertex_data, GL_ARRAY_BUFFER, handle_, element_count_);

    element_count_ = vertex_data.size();
}

void OpenGLBuffer::write(const std::vector<std::uint32_t> &index_data)
{
    update_buffer(index_data, GL_ELEMENT_ARRAY_BUFFER, handle_, element_count_);

    element_count_ = index_data.size();
}

}
