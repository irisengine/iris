#pragma once

#include <memory>
#include <vector>

#include "core/camera.hpp"
#include "graphics/material.hpp"
#include "graphics/sprite.hpp"
#include "platform/window.hpp"

namespace eng
{

/**
 * Class for rendering scenes.
 */
class render_system
{
    public:

        /**
         * Create a new opengl rendering system.
         *
         * @param cam
         *   Camera to render scene through.
         *
         * @param win
         *   Rendering window.
         */
        render_system(
            std::shared_ptr<camera> cam,
            std::shared_ptr<window> win);

        /** Declared in mm/cpp file as implementation is an incomplete file. */
        ~render_system();
        render_system(render_system&&);
        render_system& operator=(render_system&&);

        /**
         * Add a sprite to the scene.
         *
         * @param s
         *   Sprite to add.
         */
        void add(std::shared_ptr<sprite> s);

        /**
         * Render the current scene.
         */
        void render() const;

        /**
         * Set the position of the one light in the scene.
         *
         * @param position.
         *   New position of light in world space.
         */
        void set_light_position(const vector3 &position);

    private:

        /** Collection of entities in a scene to render. */
        std::vector<std::shared_ptr<sprite>> scene_;

        /** Camera to render scene through */
        std::shared_ptr<camera> camera_;

        /** Rendering window. */
        std::shared_ptr<window> window_;

        /** Light position. */
        vector3 light_position;

        /** Pointer to implementation. */
        struct implementation;
        std::unique_ptr<implementation> impl_;
};

}

