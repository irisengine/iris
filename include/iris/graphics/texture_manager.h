////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/colour.h"
#include "core/data_buffer.h"
#include "graphics/cube_map.h"
#include "graphics/texture.h"
#include "graphics/texture_usage.h"

namespace iris
{

/**
 * Abstract class for creating and managing Texture/CubeMap objects. This class handles
 * caching and lifetime management of all created objects. Implementers just
 * need to provide a graphics API specific method for creating Texture objects.
 */
class TextureManager
{
  public:
    virtual ~TextureManager() = default;

    /**
     * Load a texture from the supplied file. Will use ResourceManager.
     *
     * This function uses caching, so loading the same resource more than once
     * will return the same handle.
     *
     * @param resource
     *   File to load.
     *
     * @param usage
     *   The usage of the texture. Default is IMAGE i.e. something that will be
     *   rendered. If Texture represents something like a normal or height map
     *   the DATA should be used.
     *
     * @returns
     *   Pointer to loaded texture.
     */
    Texture *load(const std::string &resource, TextureUsage usage = TextureUsage::IMAGE);

    /**
     * Load a CubeMap from the supplied file. Will use ResourceManager.
     *
     * This function uses caching, so loading the same series of six resources more than once
     * will return the same handle.
     *
     * @param right_resource
     *   File to load for right face of cube.
     *
     * @param left_resource
     *   File to load for left face of cube.
     *
     * @param top_resource
     *   File to load for top face of cube.
     *
     * @param bottom_resource
     *   File to load for bottom face of cube.
     *
     * @param back_resource
     *   File to load for back face of cube.
     *
     * @param front_resource
     *   File to load for front face of cube.
     *
     * @returns
     *   Pointer to loaded CubeMap.
     */
    CubeMap *load(
        const std::string &right_resource,
        const std::string &left_resource,
        const std::string &top_resource,
        const std::string &bottom_resource,
        const std::string &back_resource,
        const std::string &front_resource);

    /**
     * Create a texture from a DataBuffer.
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
     *   The usage of the texture.
     *
     * @returns
     *   Pointer to created texture.
     */
    Texture *create(const DataBuffer &data, std::uint32_t width, std::uint32_t height, TextureUsage usage);

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
     * @returns
     *   Pointer to created CubeMap.
     */
    CubeMap *create(
        const DataBuffer &right_data,
        const DataBuffer &left_data,
        const DataBuffer &top_data,
        const DataBuffer &bottom_data,
        const DataBuffer &near_data,
        const DataBuffer &far_data,
        std::uint32_t width,
        std::uint32_t height);

    /**
     * Create a CubeMap with a vertical gradient.
     *
     * @param start
     *   Bottom colour.
     *
     * @param end
     *   Top colour.
     *
     * @param width
     *   Width of each image face.
     *
     * @param height
     *   Height of each image face.
     *
     * @returns
     *   Pointer to created CubeMap.
     */
    CubeMap *create(const Colour &start, const Colour &end, std::uint32_t width, std::uint32_t height);

    /**
     * Unloaded the supplied texture (if there are no other references to it).
     *
     * Normally we would want textures to stay loaded to avoid excess loads.
     * However in some cases it may be necessary to unload a texture (if we know
     * we don't want to use it again).
     *
     * This function decrements an internal reference count and will only
     * actually unload texture memory if that reference count reaches 0.
     *
     * @param texture
     *   Texture to unload.
     */
    void unload(Texture *texture);

    /**
     * Unloaded the supplied CubeMap (if there are no other references to it).
     *
     * Normally we would want CubeMaps to stay loaded to avoid excess loads.
     * However in some cases it may be necessary to unload a CubeMap (if we know
     * we don't want to use it again).
     *
     * This function decrements an internal reference count and will only
     * actually unload texture memory if that reference count reaches 0.
     *
     * @param cube_map
     *   CubeMap to unload.
     */
    void unload(CubeMap *cube_map);

    /**
     * Get a blank 1x1 white texture
     *
     * @returns
     *   Blank texture.
     */
    Texture *blank();

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
     * @returns
     *   Created texture.
     */
    virtual std::unique_ptr<Texture> do_create(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        TextureUsage usage) = 0;

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
     * @returns
     *   Created CubeMap.
     */
    virtual std::unique_ptr<CubeMap> do_create(
        const DataBuffer &right_data,
        const DataBuffer &left_data,
        const DataBuffer &top_data,
        const DataBuffer &bottom_data,
        const DataBuffer &near_data,
        const DataBuffer &far_data,
        std::uint32_t width,
        std::uint32_t height) = 0;

    /**
     * Implementors should override this method to provide implementation
     * specific unloading logic. Called automatically when a Texture is being
     * unloaded (after its reference count is zero), default is a no-op.
     *
     * @param texture
     *   Texture about to be unloaded.
     */
    virtual void destroy(Texture *texture);

    /**
     * Implementors should override this method to provide implementation
     * specific unloading logic. Called automatically when a CubeMap is being
     * unloaded (after its reference count is zero), default is a no-op.
     *
     * @param cube_map
     *  CubeMap about to be unloaded.
     */
    virtual void destroy(CubeMap *cube_map);

  private:
    /**
     * Support struct to store a loaded Texture and a reference count.
     */
    template <class T>
    struct LoadedAsset
    {
        std::size_t ref_count;
        std::unique_ptr<T> asset;
    };

    /** Collection of loaded textures. */
    std::unordered_map<std::string, LoadedAsset<Texture>> loaded_textures_;

    /** Collection of loaded CubeMaps. */
    std::unordered_map<std::string, LoadedAsset<CubeMap>> loaded_cube_maps_;
};

}
