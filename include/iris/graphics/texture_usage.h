#pragma once

#include <cstdint>

namespace iris
{

/**
 * Encapsulation of the semantic usage of textures.
 */
enum class TextureUsage : std::uint8_t
{
    IMAGE,
    DATA,
    RENDER_TARGET,
    DEPTH
};

}
