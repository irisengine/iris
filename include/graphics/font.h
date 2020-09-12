#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "core/vector3.h"
#include "graphics/sprite.h"

namespace iris
{

/**
 * Class encapsulating a loaded Font which can can produce a Sprite containing
 * rendered text.
 */
class Font : public Sprite
{
    public:

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
        Font(
            const std::string &font_name,
            const std::uint32_t size,
            const std::string &text,
            const Vector3 &colour);

        /** Declared in mm/cpp file as implementation is an incomplete file. */
        ~Font();

    private:

        /** Name of font. */
        std::string font_name_;

        /** Colour of text. */
        Vector3 colour_;

        /** Pointer to platform specific implementation details. */
        struct implementation;
        std::unique_ptr<implementation> impl_;
};

}

