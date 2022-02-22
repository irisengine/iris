////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>

#include "core/error_handling.h"
#include "graphics/opengl/opengl.h"
#include "graphics/vertex_data.h"

namespace iris
{

/**
 * This class encapsulates an opengl buffer template on target and usage.
 */
template <GLenum Target, GLenum Usage, bool FixedSize>
class OpenGLBuffer
{
  public:
    /**
     * Construct a new Open GLBuffer object with an initial capacity.
     *
     * @param capacity
     *   Capacity of buffer.
     */
    OpenGLBuffer(std::size_t capacity)
        : handle_(0u)
        , capacity_(capacity)
    {
        ::glGenBuffers(1, &handle_);
        iris::expect(iris::check_opengl_error, "could not generate opengl buffer");

        ::glBindBuffer(Target, handle_);
        expect(check_opengl_error, "could not bind buffer");

        ::glBufferData(Target, capacity_, nullptr, Usage);
        expect(check_opengl_error, "could not create buffer");

        ::glBindBuffer(Target, 0u);
        expect(check_opengl_error, "could not unbind buffer");
    }

    /**
     * Construct a new Open GLBuffer object with an initial capacity.
     *
     * @param capacity
     *   Capacity of buffer.
     *
     * @param index
     *   Index of binding point in Target.
     */
    OpenGLBuffer(std::size_t capacity, GLuint index)
        : handle_(0u)
        , capacity_(capacity)
    {
        ::glGenBuffers(1, &handle_);
        iris::expect(iris::check_opengl_error, "could not generate opengl buffer");

        ::glBindBuffer(Target, handle_);
        expect(check_opengl_error, "could not bind buffer");

        ::glBufferData(Target, capacity_, nullptr, Usage);
        expect(check_opengl_error, "could not create buffer");

        ::glBindBuffer(Target, 0u);
        expect(check_opengl_error, "could not unbind buffer");

        ::glBindBufferRange(Target, index, handle_, 0, capacity_);
        expect(check_opengl_error, "could not set buffer range");
    }

    ~OpenGLBuffer()
    {
        ::glDeleteBuffers(1, &handle_);
    }

    OpenGLBuffer(const OpenGLBuffer &) = delete;
    OpenGLBuffer &operator=(const OpenGLBuffer &) = delete;

    /**
     * Get the opengl handle to the buffer.
     *
     * @returns
     *   OpenGL handle.
     */
    GLuint handle() const
    {
        return handle_;
    }

    /**
     * Get capacity of buffer.
     *
     * @returns
     *   Buffer capacity.
     */
    std::size_t capacity() const
    {
        return capacity_;
    }

    /**
     * Write an object into the buffer at an offset.
     *
     * @param object
     *   Object to write.
     *
     * @param offset
     *   Offset into buffer to write object.
     */
    template <class T>
    void write(const T &object, std::size_t offset)
    {
        write(std::addressof(object), sizeof(T), offset);
    }

    /**
     * Write an object into the buffer at an offset.
     *
     * If offset + size > capacity then the behaviour depends on FixedSize:
     *   true - throws
     *   false - resizes the buffer
     *
     * @param object
     *   Object to write.
     *
     * @param size
     *   Size (in bytes) of object to write.
     *
     * @param offset
     *   Offset into buffer to write object.
     */
    template <class T>
    void write(const T *object, std::size_t size, std::size_t offset)
    {
        if (offset + size > capacity_)
        {
            if constexpr (FixedSize)
            {
                throw Exception("write would overflow");
            }
            else
            {
                // pick some sensible new capacity size
                const auto old_capacity = capacity_;
                capacity_ = std::max(offset + size, capacity_ * 2u);

                // bind current handle to copy target
                ::glBindBuffer(GL_COPY_READ_BUFFER, handle_);
                expect(check_opengl_error, "could not bind read buffer");

                // create a new handle
                GLuint new_handle = 0u;
                ::glGenBuffers(1u, &new_handle);
                expect(check_opengl_error, "could not create new buffer");

                // bind new handle to existing target
                ::glBindBuffer(Target, new_handle);
                expect(check_opengl_error, "could not bind write buffer");

                // allocate space for new handle
                ::glBufferData(Target, capacity_, nullptr, Usage);
                expect(check_opengl_error, "could not allocate write buffer");

                // copy data from old handle to new handle
                ::glCopyBufferSubData(GL_COPY_READ_BUFFER, Target, 0, 0, old_capacity);
                expect(check_opengl_error, "could not copy data");

                // delete old handle
                ::glDeleteBuffers(1, &handle_);
                expect(check_opengl_error, "could not delete old handle");

                // update new handle - note that if this buffer is a VBO or EBO it will invalidate any VAOs
                handle_ = new_handle;

                ::glBindBuffer(GL_COPY_READ_BUFFER, 0u);
                expect(check_opengl_error, "could not unbind read buffer");
            }
        }

        ::glBindBuffer(Target, handle_);
        expect(check_opengl_error, "could not bind buffer");

        ::glBufferSubData(Target, offset, size, object);
        expect(check_opengl_error, "could not set buffer data");

        ::glBindBuffer(Target, 0u);
        expect(check_opengl_error, "could not unbind buffer");
    }

  private:
    /** OpenGL handle for buffer. */
    GLuint handle_;

    /** Capacity of buffer. */
    std::size_t capacity_;
};

// helper aliases
using VBO = OpenGLBuffer<GL_ARRAY_BUFFER, GL_STATIC_DRAW, false>;
using EBO = OpenGLBuffer<GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, false>;
using UBO = OpenGLBuffer<GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW, true>;
using SSBO = OpenGLBuffer<GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, true>;

}