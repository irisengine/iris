#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "core/vector3.hpp"
#include "graphics/sprite.hpp"

namespace eng
{

/**
 * Class encapsulating a loaded Font which can can produce a Sprite containing
 * rendered text.
 */
class Font
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
         * @param colour
         *   The colour of the font.
         */
        Font(
            const std::string &font_name,
            const std::uint32_t size,
            const Vector3 &colour);

        /** Declared in mm/cpp file as implementation is an incomplete file. */
        ~Font();
        Font(Font&&);
        Font& operator=(Font&&);

        /**
         * Create a Sprite which, when rendered, will draw the
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
        std::unique_ptr<Sprite> sprite(
            const std::string &text,
            const float x,
            const float y) const;

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

