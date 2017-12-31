#pragma once

#include <vector>

#include "camera.hpp"
#include "entity.hpp"
#include "material.hpp"
#include "matrix.hpp"

namespace eng
{

/**
 * Class for rendering scenes.
 */
class render_system final
{
    public:

        /**
         * Create a new opengl rendering system.
         *
         * @param c
         *   Camera to render scene through.
         *
         * @param width
         *   Width of render window.
         *
         * @param height
         *   Height of render window.
         */
        render_system(
            std::shared_ptr<camera> c,
            const float width,
            const float height);

        /** Default */
        ~render_system() = default;
        render_system(render_system&&) = default;
        render_system& operator=(render_system&&) = default;

        /** Disabled */
        render_system(const render_system&) = delete;
        render_system& operator=(const render_system&) = delete;

        /**
         * Add an entity to the scene.
         *
         * @param e
         *   Entity to add.
         */
        void add(std::shared_ptr<entity> e);

        /**
         * Render the current scene.
         */
        void render() const;

        /**
         * Set whether the scene should be drawn in wireframe mode.
         *
         * @param wireframe
         *   True if scene should be rendered in wireframe mode, false otherwise.
         */
        void set_wireframe_mode(bool wireframe);

    private:

        /** Collection of entities in a scene to render. */
        std::vector<std::shared_ptr<entity>> scene_;

        /** Camera to render scene through */
        std::shared_ptr<camera> camera_;

        /** Global material. */
        material material_;
};

}

