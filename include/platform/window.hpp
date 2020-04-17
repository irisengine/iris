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
class window
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
        window(
            const float with,
            const float height);

        /** Disabled */
        window(const window&) = delete;
        window& operator=(const window&) = delete;

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
        sprite* create(
            const float x,
            const float y,
            const float width,
            const float height,
            const vector3 &colour);

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
        sprite* create(
            const float x,
            const float y,
            const float width,
            const float height,
            const vector3 &colour,
            texture &&tex);

        /**
         * Pump the next user input event. Result will be empty if there are no
         * new events.
         *
         * @returns
         *   Optional event.
         */
        std::optional<event> pump_event();

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
        std::unique_ptr<render_system> render_system_;
        
        /** Queue of input events. */
        std::queue<event> events_;

        /** Window width. */
        float width_;

        /** Window height. */
        float height_;
};

}

