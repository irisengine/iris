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
#include "graphics/cube_map.h"
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
     *
     * @param index
     *   Index into the global array of all allocated textures.
     *
     * @returns
     *   Created texture.
     */
    std::unique_ptr<Texture> do_create(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        TextureUsage usage,
        std::uint32_t index) override;

    /**
     * Create a CubeMap from six DataBuffers (one for each face).
     *
     * @param right_data
     *   Image data for right face of cube.
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
     * @param index
     *   Index into the global array of all allocated textures.
     *
     * @returns
     *   Created CubeMap.
     */
    std::unique_ptr<CubeMap> do_create(
        const DataBuffer &right_data,
        const DataBuffer &left_data,
        const DataBuffer &top_data,
        const DataBuffer &bottom_data,
        const DataBuffer &near_data,
        const DataBuffer &far_data,
        std::uint32_t width,
        std::uint32_t height,
        std::uint32_t index) override;

    /**
     * Unload a texture.
     *
     * @param texture
     *   Texture about to be unloaded.
     */
    void destroy(Texture *texture) override;

    /**
     * Unload a CubeMap.
     *
     * @param cube_map
     *   CubeMap about to be unloaded.
     */
    void destroy(CubeMap *cube_map) override;

  private:
    /** Stack of available ids. */
    std::stack<GLuint> id_pool_;
};

}
