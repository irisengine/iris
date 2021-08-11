#pragma once

#include <cstdint>
#include <memory>
#include <stack>

#include "core/data_buffer.h"
#include "graphics/opengl/opengl.h"
#include "graphics/pixel_format.h"
#include "graphics/texture_manager.h"

namespace iris
{

/**
 * Implementation of TextureManager for OpenGL.
 */
class OpenGLTextureManager : public TextureManager
{
  public:
    /**
     * Construct a new OpenGLTextureManager.
     */
    OpenGLTextureManager();

    ~OpenGLTextureManager() override = default;

    /**
     * Get the next texture unit id from a pool of available ids.
     *
     * @returns
     *   Next available texture id.
     */
    GLuint next_id();

    /**
     * Return an id to the pool.
     *
     * @param id
     *   Id to return to pool.
     */
    void return_id(GLuint id);

  protected:
    /**
     * Create a Texture object with the provided data.
     *
     * @param data
     *   Raw data of image, in pixel_format.
     *
     * @param width
     *   Width of image.
     *
     * @param height
     *   Height of image.
     *
     * @param pixel_format
     *   Format of pixel data.
     */
    std::unique_ptr<Texture> create(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        PixelFormat pixel_format) override;

    /**
     * Unload a texture.
     *
     * @param texture
     *   Texture about to be unloaded.
     */
    void destroy(Texture *texture) override;

  private:
    /** Stack of available ids. */
    std::stack<GLuint> id_pool_;
};

}
