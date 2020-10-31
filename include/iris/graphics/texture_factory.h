#pragma once

#include <string>

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
 * Get a blank 1x1 white texture
 *
 * @returns
 *   Blank texture.
 */
Texture *blank();

}
