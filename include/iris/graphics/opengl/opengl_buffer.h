////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "graphics/opengl/opengl.h"
#include "graphics/vertex_data.h"

namespace iris
{

/**
 * This class encapsulates an opengl buffer. A buffer can be created with either
 * vertex or index data.
 */
class OpenGLBuffer
{
  public:
    /**
     * Construct a new OpenGLBuffer with vertex data.
     *
     * @param vertex_data
     *   Vertex data to copy to buffer.
     */
    OpenGLBuffer(const std::vector<VertexData> &vertex_data);

    /**
     * Construct a new OpenGLBuffer with index data.
     *
     * @param vertex_data
     *   Index data to copy to buffer.
     */
    OpenGLBuffer(const std::vector<std::uint32_t> &index_data);

    /**
     * Clean up opengl objects.
     */
    ~OpenGLBuffer();

    OpenGLBuffer(const OpenGLBuffer &) = delete;
    OpenGLBuffer &operator=(const OpenGLBuffer &) = delete;

    /**
     * Get the opengl handle to the buffer.
     *
     * @returns
     *   OpenGL handle.
     */
    GLuint handle() const;

    /**
     * Get the number of elements stored in the buffer.
     *
     * @returns
     *   Number of elements in buffer.
     */
    std::size_t element_count() const;

    /**
     * Write vertex data to the buffer.
     *
     * @param vertex_data
     *   New vertex data.
     */
    void write(const std::vector<VertexData> &vertex_data);

    /**
     * Write index data to the buffer.
     *
     * @param index_data
     *   New index data.
     */
    void write(const std::vector<std::uint32_t> &index_data);

  private:
    /** OpenGL handle for buffer. */
    GLuint handle_;

    /** Number of elements in buffer. */
    std::size_t element_count_;
};

}