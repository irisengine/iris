#pragma once

#include <memory>

#include "graphics/render_system.hpp"
#include "graphics/sprite.hpp"
#include "platform/event_dispatcher.hpp"

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
         * @param dispatcher
         *   An object used for dispatching input event.
         *
         * @param width
         *   Width of the window.
         *
         * @param height
         *   Height of the window.
         */
        window(
            event_dispatcher &dispatcher,
            const float with,
            const float height);

        /** Disabled */
        window(const window&) = delete;
        window& operator=(const window&) = delete;

        /**
         * Render the current scene.
         */
        void render() const;

        /**
         * Add a sprite to the scene.
         *
         * @param s
         *   Sprite to add.
         */
        void add(std::shared_ptr<sprite> s);

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

        /** Reference to an input dispatcher. */
        event_dispatcher &dispatcher_;

        /** Render system for window. */
        std::unique_ptr<render_system> render_system_;

        /** Window width. */
        float width_;

        /** Window height. */
        float height_;
};

}

