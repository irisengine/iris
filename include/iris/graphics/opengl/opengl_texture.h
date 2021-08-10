#pragma once

#include <cstdint>

#include "graphics/opengl/opengl.h"

#include "core/data_buffer.h"
#include "graphics/pixel_format.h"
#include "graphics/texture.h"

namespace iris
{

/**
 * Implementation of Texture for OpenGL.
 */
class OpenGLTexture : public Texture
{
  public:
    /**
     * Construct a new OpenGLTexture.
     *
     * @param data
     *   Image data. This should be width * hight of pixel_format tuples.
     *
     * @param width
     *   Width of image.
     *
     * @param height
     *   Height of data.
     *
     * @param id
     *    OpenGL texture unit.
     */
    OpenGLTexture(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        PixelFormat pixel_format,
        GLuint id);

    /**
     * Clean up OpenGL objects.
     */
    ~OpenGLTexture() override;

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

  private:
    /** OpenGL texture handle. */
    GLuint handle_;

    /** OpenGL texture unit. */
    GLuint id_;
};

}
