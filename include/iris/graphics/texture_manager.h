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
#include <unordered_set>
#include <vector>

#include "core/colour.h"
#include "core/data_buffer.h"
#include "graphics/cube_map.h"
#include "graphics/sampler.h"
#include "graphics/texture.h"
#include "graphics/texture_usage.h"

namespace iris
{

/**
 * Abstract class for creating and managing Texture/CubeMap objects. This class handles caching and lifetime management
 * of all created objects. Implementers just need to provide a graphics API specific method for creating Texture
 * objects.
 */
class TextureManager
{
  public:
    TextureManager();
    virtual ~TextureManager() = default;

    /**
     * Load a texture from the supplied file. Will use ResourceManager.
     *
     * This function uses caching, so loading the same resource more than once will return the same handle.
     *
     * @param resource
     *   File to load.
     *
     * @param usage
     *   The usage of the texture. Default is IMAGE i.e. something that will be rendered. If Texture represents
     *   something like a normal or height map the DATA should be used.
     *
     * @param sampler
     *   Sampler to sue for texture, if nullptr the default sampler will be used.
     *
     * @returns
     *   Pointer to loaded texture.
     */
    Texture *load(
        const std::string &resource,
        TextureUsage usage = TextureUsage::IMAGE,
        const Sampler *sampler = nullptr);

    /**
     * Load a CubeMap from the supplied file. Will use ResourceManager.
     *
     * This function uses caching, so loading the same series of six resources more than once will return the same
     * handle.
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
     * @param sampler
     *   Sampler to sue for texture, if nullptr the default sampler will be used.
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
        const std::string &front_resource,
        const Sampler *sampler = nullptr);

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
     * @param sampler
     *   Sampler to sue for texture, if nullptr the default sampler will be used.
     *
     * @returns
     *   Pointer to created texture.
     */
    Texture *create(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        TextureUsage usage,
        const Sampler *sampler = nullptr);

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
     * @param sampler
     *   Sampler to sue for texture, if nullptr the default sampler will be used.
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
        std::uint32_t height,
        const Sampler *sampler = nullptr);

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
     * @param sampler
     *   Sampler to sue for texture, if nullptr the default sampler will be used.
     *
     * @returns
     *   Pointer to created CubeMap.
     */
    CubeMap *create(
        const Colour &start,
        const Colour &end,
        std::uint32_t width,
        std::uint32_t height,
        const Sampler *sampler = nullptr);

    /**
     * Create a Sampler.
     *
     * @param descriptor
     *   Description of sampler.
     *
     * @returns
     *   Pointer to created Sampler.
     */
    Sampler *create(const SamplerDescriptor &descriptor);

    /**
     * Unloaded the supplied texture (if there are no other references to it).
     *
     * Normally we would want textures to stay loaded to avoid excess loads. However in some cases it may be necessary
     * to unload a texture (if we know we don't want to use it again).
     *
     * This function decrements an internal reference count and will only actually unload texture memory if that
     * reference count reaches 0.
     *
     * @param texture
     *   Texture to unload.
     */
    void unload(const Texture *texture);

    /**
     * Unloaded the supplied CubeMap (if there are no other references to it).
     *
     * Normally we would want CubeMaps to stay loaded to avoid excess loads. However in some cases it may be necessary
     * to unload a CubeMap (if we know we don't want to use it again).
     *
     * This function decrements an internal reference count and will only actually unload texture memory if that
     * reference count reaches 0.
     *
     * @param cube_map
     *   CubeMap to unload.
     */
    void unload(const CubeMap *cube_map);

    /**
     * Unloaded the supplied Sampler (if there are no other references to it).
     *
     * Normally we would want samplers to stay loaded to avoid excess loads. However in some cases it may be necessary
     * to unload a sampler (if we know we don't want to use it again).
     *
     * This function decrements an internal reference count and will only actually unload if that reference count
     * reaches 0.
     *
     * @param sampler
     *   Sampler to unload.
     */
    void unload(const Sampler *sampler);

    /**
     * Get a blank 1x1 white texture
     *
     * @returns
     *   Blank texture.
     */
    Texture *blank_texture();

    /**
     * Get a blank white cube_map
     *
     * @returns
     *   Blank cube map.
     */
    CubeMap *blank_cube_map();

    /**
     * Default sampler to use for textures.
     *
     * @returns
     *   Default sampler.
     */
    Sampler *default_texture_sampler();

    /**
     * Default sampler to use for cube maps.
     *
     * @returns
     *   Default sampler.
     */
    Sampler *default_cube_map_sampler();

    /**
     * Get the next available index for a new texture.
     *
     * @returns
     *   Next available index.
     */
    std::uint32_t next_texture_index();

    /**
     * Get the next available index for a new cube map.
     *
     * @returns
     *   Next available index.
     */
    std::uint32_t next_cube_map_index();

    /**
     * Get the next available index for a new sampler.
     *
     * @returns
     *   Next available index.
     */
    std::uint32_t next_sampler_index();

    /**
     * Get a copy of all Texture pointers (sorted by index).
     *
     * @return
     *   All Texture pointers.
     */
    std::vector<const Texture *> textures() const;

    /**
     * Get a copy of all CubeMap pointers (sorted by index).
     *
     * @return
     *   All CubeMap pointers.
     */
    std::vector<const CubeMap *> cube_maps() const;

    /**
     * Get a copy of all Sampler pointers (sorted by index).
     *
     * @return
     *   All Sampler pointers.
     */
    std::vector<const Sampler *> samplers() const;

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
     * @param sampler
     *   Sampler to use for this texture.
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
    virtual std::unique_ptr<Texture> do_create(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        const Sampler *sampler,
        TextureUsage usage,
        std::uint32_t index) = 0;

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
     * @param sampler
     *   Sampler to use for this texture.
     *
     * @param index
     *   Index into the global array of all allocated textures.
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
        std::uint32_t height,
        const Sampler *sampler,
        std::uint32_t index) = 0;

    virtual std::unique_ptr<Sampler> do_create(const SamplerDescriptor &descriptor, std::uint32_t index) = 0;

    /**
     * Implementers should override this method to provide implementation specific unloading logic. Called automatically
     * when a Texture is being unloaded (after its reference count is zero), default is a no-op.
     *
     * @param texture
     *   Texture about to be unloaded.
     */
    virtual void destroy(const Texture *texture);

    /**
     * Implementers should override this method to provide implementation specific unloading logic. Called automatically
     * when a CubeMap is being unloaded (after its reference count is zero), default is a no-op.
     *
     * @param cube_map
     *  CubeMap about to be unloaded.
     */
    virtual void destroy(const CubeMap *cube_map);

    /**
     * Implementers should override this method to provide implementation specific unloading logic. Called automatically
     * when a Sampler is being unloaded (after its reference count is zero), default is a no-op.
     *
     * @param sampler
     *  Sampler about to be unloaded.
     */
    virtual void destroy(const Sampler *sampler);

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

    /** Collection of loaded Samplers. */
    std::unordered_map<SamplerDescriptor, LoadedAsset<Sampler>> loaded_samplers_;

    /** Next index to use (if free list is empty). */
    std::uint32_t texture_index_counter_;

    /** Collection of returned indices (which will be recycled). */
    std::vector<std::uint32_t> texture_index_free_list_;

    /** Next index to use (if free list is empty). */
    std::uint32_t cube_map_index_counter_;

    /** Collection of returned indices (which will be recycled). */
    std::vector<std::uint32_t> cube_map_index_free_list_;

    /** Next index to use (if free list is empty). */
    std::uint32_t sampler_index_counter_;

    /** Collection of returned indices (which will be recycled). */
    std::vector<std::uint32_t> sampler_index_free_list_;
};

}
