#pragma once

#include <memory>
#include <optional>
#include <queue>

#include "core/camera.h"
#include "core/real.h"
#include "graphics/render_system.h"
#include "graphics/sprite.h"
#include "platform/event.h"

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
        Window(real with, real height);

        /** Disabled */
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        /**
         * Render the current scene.
         */
        void render();

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
        real width() const;

        /**
         * Get the height of the window.
         *
         * @returns
         *   Window height.
         */
        real height() const;

    private:

        /** Window width. */
        real width_;

        /** Window height. */
        real height_;
};

}

