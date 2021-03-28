#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "core/data_buffer.h"
#include "graphics/pixel_format.h"
#include "graphics/texture.h"

namespace iris
{

class TextureManager
{
  public:
    /**
     * Load a texture from the supplied file. Will use ResourceManager.
     *
     * This function uses caching, so loading the same resource more than once
     * will return the same handle.
     *
     * @param resource
     *   File to load.
     *
     * @returns
     *   Pointer to loaded texture.
     */
    static Texture *load(const std::string &resource);

    /**
     * Load a texture from a DataBuffer.
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
    static Texture *load(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        PixelFormat pixel_foramt);

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
    static void unload(Texture *texture);

    /**
     * Get a blank 1x1 white texture
     *
     * @returns
     *   Blank texture.
     */
    static Texture *blank();

  private:
    /**
     * Create a new TextureManager. Private to force use of static functions.
     */
    TextureManager();

    /**
     * Get the single instance of this class. Private to force use of static
     * functions.
     *
     * @returns
     *   Single instance.
     */
    static TextureManager &instance();

    // these methods are implementations of the above static methods - see their
    // doc comments for details

    Texture *load_impl(const std::string &resource);

    Texture *load_impl(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        PixelFormat pixel_foramt);

    void unload_impl(Texture *texture);

    /**
     * Support struct to store a loaded Texture and a reference count.
     */
    struct LoadedTexture
    {
        std::size_t ref_count;
        std::unique_ptr<Texture> texture;
    };

    /** Collection of loaded textures. */
    std::map<std::string, LoadedTexture> loaded_textures_;
};

}
