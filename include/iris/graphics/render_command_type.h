#pragma once

#include <cstdint>

namespace iris
{

/**
 * Enumeration of possible render command types.
 */
enum class RenderCommandType : std::uint8_t
{
    UPLOAD_TEXTURE,
    PASS_START,
    DRAW,
    PASS_END,
    PRESENT
};

}