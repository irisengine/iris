////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>
#include <stack>

#include "core/data_buffer.h"
#include "graphics/opengl/opengl.h"
#include "graphics/texture_manager.h"
#include "graphics/texture_usage.h"

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
     * @param usage
     *   Usage of the texture.
     */
    std::unique_ptr<Texture> do_create(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        TextureUsage usage) override;

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
