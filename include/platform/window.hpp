#pragma once

#include <memory>
#include <optional>
#include <queue>

#include "graphics/render_system.hpp"
#include "graphics/sprite.hpp"
#include "platform/event.hpp"

namespace eng
{

/**
 * Class representing a native window.
 */
class Window
{
    public:

        /**
         * Create and display a new native window.
         *
         * @param width
         *   Width of the window.
         *
         * @param height
         *   Height of the window.
         */
        Window(
            const float with,
            const float height);

        /** Disabled */
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        /**
         * Render the current scene.
         */
        void render();

        /**
         * Create a solid colour sprite. The lifetime of this pointer is
         * automatically managed by the engine. *DO NOT* delete/free it.
         *
         * @param x
         *   Screen x coordinate of centre of sprite.
         *
         * @param y
         *   Screen y coordinate of centre of sprite.
         *
         * @param width
         *   Width of sprite.
         *
         * @param height
         *   Height of sprite.
         *
         * @param colour
         *   Colour of sprite.
         *
         *  @returns
         *    A pointer to the newly created sprite.
         */
        Sprite* create(
            const float x,
            const float y,
            const float width,
            const float height,
            const Vector3 &colour);

        /**
         * Create a textured sprite. The lifetime of this pointer is
         * automatically managed by the engine. *DO NOT* delete/free it.
         *
         * @param x
         *   Screen x coordinate of centre of sprite.
         *
         * @param y
         *   Screen y coordinate of centre of sprite.
         *
         * @param width
         *   Width of sprite.
         *
         * @param height
         *   Height of sprite.
         *
         * @param colour
         *   Colour of sprite.
         *
         * @param tex
         *   Texture of sprite.
         *
         *  @returns
         *    A pointer to the newly created sprite.
         */
        Sprite* create(
            const float x,
            const float y,
            const float width,
            const float height,
            const Vector3 &colour,
            Texture &&tex);

        /**
         * Create a Sprite which renders text in the supplied font, size and
         * colour. The lifetime of this pointer is automatically managed by the
         * engine. *DO NOT* delete/free it.
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
         *
         * @param text
         *   Text to render.
         *
         * @param x
         *   x coordinate of centre of rendered text.
         *
         * @param y
         *   y coordinate of centre of rendered text.
         *
         *  @returns
         *    A pointer to the newly created sprite.
         */
        Sprite* create(
            const std::string &font_name,
            const std::uint32_t size,
            const Vector3 &colour,
            const std::string &text,
            const float x,
            const float y);

        /**
         * Pump the next user input event. Result will be empty if there are no
         * new events.
         *
         * @returns
         *   Optional event.
         */
        std::optional<Event> pump_event();

        /**
         * Get the width of the window.
         *
         * @returns
         *   Window width.
         */
        float width() const;

        /**
         * Get the height of the window.
         *
         * @returns
         *   Window height.
         */
        float height() const;

    private:

        /** Render system for window. */
        std::unique_ptr<RenderSystem> render_system_;
        
        /** Queue of input events. */
        std::queue<Event> events_;

        /** Window width. */
        float width_;

        /** Window height. */
        float height_;
};

}

