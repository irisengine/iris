#pragma once

#include <cstdint>

namespace iris
{

/**
 * Encapsulation of pixel formats i.e. how pixel data can be represented in
 * textures.
 */
enum class PixelFormat : std::uint8_t
{
    /** Single colour component. */
    R,
    /** Red, green, blue components. */
    RGB,
    /** Red, green, blue, alpha components. */
    RGBA,
    /** Special format for storing depth information. */
    DEPTH
};

}
