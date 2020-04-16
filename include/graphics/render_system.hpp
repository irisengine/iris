#pragma once

#include <memory>
#include <vector>

#include "core/camera.hpp"
#include "graphics/material.hpp"
#include "graphics/sprite.hpp"

namespace eng
{

/**
 * Class for rendering scenes.
 */
class render_system
{
    public:

        /**
         * Create a new rendering system.
         */
        render_system();

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
        camera camera_;

        /** Light position. */
        vector3 light_position;

        /** Pointer to implementation. */
        struct implementation;
        std::unique_ptr<implementation> impl_;
};

}

