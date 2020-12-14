#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "core/vector3.h"
#include "graphics/scene.h"

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
std::unique_ptr<Scene> create(
    const std::string &font_name,
    std::uint32_t size,
    const std::string &text,
    const Vector3 &colour);

}
