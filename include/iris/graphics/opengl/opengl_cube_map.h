////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#include "core/data_buffer.h"
#include "graphics/cube_map.h"
#include "graphics/opengl/opengl.h"
#include "graphics/sampler.h"

namespace iris
{

/**
 * Implementation of CubeMap for OpenGL.
 */
class OpenGLCubeMap : public CubeMap
{
  public:
    /**
     * Construct a new OpenGLCubeMap.
     *
     * @param right_data
     *   Image data for right face of cube.
     *
     * @param left_data
     *   Image data for left face of cube.
     *
     * @param top_data
     *   Image data for top face of cube.
     *
     * @param bottom_data
     *   Image data for bottom face of cube.
     *
     * @param back_data
     *   Image data for back face of cube.
     *
     * @param front_data
     *   Image data for front face of cube.
     *
     * @param width
     *   Width of each image face.
     *
     * @param height
     *   Height of each image face.
     *
     * @param sampler
     *   Sampler to use for this cube map.
     *
     * @param index
     *   Index into the global array of all allocated textures.
     *
     * @param id
     *    OpenGL texture unit.
     */
    OpenGLCubeMap(
        const DataBuffer &right_data,
        const DataBuffer &left_data,
        const DataBuffer &top_data,
        const DataBuffer &bottom_data,
        const DataBuffer &back_data,
        const DataBuffer &front_data,
        std::uint32_t width,
        std::uint32_t height,
        const Sampler *sampler,
        std::uint32_t index,
        GLuint id);

    /**
     * Clean up OpenGL objects.
     */
    ~OpenGLCubeMap() override;

    /**
     * Get OpenGL handle to texture.
     *
     * @returns
     *   OpenGL texture handle.
     */
    GLuint handle() const;

    /**
     * Get OpenGL texture unit.
     *
     * @returns
     *   OpenGL texture unit.
     */
    GLuint id() const;

    /**
     * Get the OpenGL bindless handle for this cube map.
     *
     * @returns
     *   Bindless handle.
     */
    GLuint64 bindless_handle() const;

  private:
    /** OpenGL texture handle. */
    GLuint handle_;

    /** OpenGL texture unit. */
    GLuint id_;

    /** OpengGL bindless handle. */
    GLuint64 bindless_handle_;
};

}
