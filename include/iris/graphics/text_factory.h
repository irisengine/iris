////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "core/colour.h"
#include "graphics/texture.h"

namespace iris::text_factory
{

/**
 * Construct a new font.
 *
 * @param font_name
 *   The name of a Font to load. This is located and loaded in a
 *   platform specific way, so the Font must exist for the current
 *   platform.
 *
 * @param size
 *   The Font size.
 *
 * @param text
 *   The text to render.
 *
 * @param colour
 *   The colour of the font.
 */
Texture *create(const std::string &font_name, std::uint32_t size, const std::string &text, const Colour &colour);

}
