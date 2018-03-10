#pragma once

#include "event_dispatcher.hpp"

namespace eng
{

/**
 * Class representing a native OpenGl window.
 */
class window final
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

        /** Default */
        ~window() = default;
        window(window&&) = default;
        window& operator=(window&&) = default;

        /** Disabled */
        window(const window&) = delete;
        window& operator=(const window&) = delete;

        /**
         * Perform any actions required before rendering.
         */
        void pre_render() const noexcept;

        /**
         * Perform any actions required after rendering.
         */
        void post_render() const noexcept;

        /**
         * Get the width of the window.
         *
         * @returns
         *   Window width.
         */
        float width() const noexcept;

        /**
         * Get the height of the window.
         *
         * @returns
         *   Window height.
         */
        float height() const noexcept;

    private:

        /** Referecne to an input dispatcher */
        event_dispatcher &dispatcher_;

        /** Window width. */
        float width_;

        /** Window height. */
        float height_;
};

}

