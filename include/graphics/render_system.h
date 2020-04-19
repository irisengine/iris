#pragma once

#include <memory>
#include <vector>

#include "core/camera.h"
#include "core/vector3.h"
#include "graphics/material.h"
#include "graphics/sprite.h"

namespace eng
{

/**
 * Class for rendering scenes.
 */
class RenderSystem
{
    public:

        /**
         * Create a new rendering system.
         *
         * @param width
         *   Width of window.
         *
         * @param height
         *   Height of window.
         */
        RenderSystem(float width, float height);

        /** Declared in mm/cpp file as implementation is an incomplete file. */
        ~RenderSystem();
        RenderSystem(RenderSystem&&);
        RenderSystem& operator=(RenderSystem&&);

        /**
         * Create a Sprite and add it to the scene. Uses perfect forwarding to
         * pass along all arguments.
         *
         * @param args
         *   Arguments for sprite.
         *
         * @returns
         *    A pointer to the newly created sprite.
         */
        template<class ...Args>
        Sprite* create(Args &&...args)
        {
            scene_.emplace_back(std::make_unique<Sprite>(std::forward<Args>(args)...));
            return scene_.back().get();
        }

        /**
         * Add a Sprite to the scene.
         *
         * @param sprite
         *   Sprite to render.
         *
         * @returns
         *   Pointer to the added sprite.
         */
        Sprite* add(std::unique_ptr<Sprite> sprite);

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
        void set_light_position(const Vector3 &position);

    private:

        /** Collection of entities in a scene to render. */
        std::vector<std::unique_ptr<Sprite>> scene_;

        /** Camera to render scene through */
        Camera camera_;

        /** Light position. */
        Vector3 light_position;

        /** Pointer to implementation. */
        struct implementation;
        std::unique_ptr<implementation> impl_;
};

}

