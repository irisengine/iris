#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "sprite.hpp"
#include "vector3.hpp"

namespace eng
{

/**
 * Class encapsulating a loaded font which can can produce a sprite containing
 * rendered text.
 */
class font
{
    public:

        /**
         * Construct a new font.
         *
         * @param font_name
         *   The name of a font to load. This is located and loaded in a
         *   platform specific way, so the font must exist for the current
         *   platform.
         *
         * @param size
         *   The font size.
         *
         * @param colour
         *   The colour of the font.
         */
        font(
            const std::string &font_name,
            const std::uint32_t size,
            const vector3 &colour);

        /** Declared in mm/cpp file as implementation is an incomplete file. */
        ~font();
        font(font&&);
        font& operator=(font&&);

        /**
         * Create a sprite which, when rendered, will draw the
         * supplied text.
         *
         * @param text
         *   Text to render.
         *
         * @param x
         *   x coordinate of centre of rendered text.
         *
         * @param y
         *   y coordinate of centre of rendered text.
         */
        std::shared_ptr<sprite> sprites(
            const std::string &text,
            const float x,
            const float y);

    private:

        /** Name of font. */
        std::string font_name_;

        /** Colour of text. */
        vector3 colour_;

        /** Pointer to platform specific implementation details. */
        struct implementation;
        std::unique_ptr<implementation> impl_;
};

}

