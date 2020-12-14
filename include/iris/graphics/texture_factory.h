#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "graphics/pixel_format.h"
#include "graphics/texture.h"

namespace iris::texture_factory
{

/**
 * Load a texture from the supplied file. Will use ResourceManager.
 *
 * This function uses caching, so loading the same resource more than once will
 * return the same handle.
 *
 * @param resource
 *   File to load.
 *
 * @returns
 *   Pointer to loaded texture.
 */
Texture *load(const std::string &resource);

/**
 * Creates a new Texture with custom data.
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
Texture *create(
    const std::vector<std::uint8_t> &data,
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_foramt);

/**
 * Get a blank 1x1 white texture
 *
 * @returns
 *   Blank texture.
 */
Texture *blank();

}
