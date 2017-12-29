#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include "camera.hpp"
#include "entity.hpp"
#include "gl/material.hpp"
#include "matrix.hpp"

namespace eng
{

/**
 * Class for rendering scenes with opengl.
 */
class opengl_render_system final
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
        opengl_render_system(
            std::shared_ptr<camera> c,
            const float width,
            const float height);

        /** Default */
        ~opengl_render_system() = default;
        opengl_render_system(opengl_render_system&&) = default;
        opengl_render_system& operator=(opengl_render_system&&) = default;

        /** Disabled */
        opengl_render_system(const opengl_render_system&) = delete;
        opengl_render_system& operator=(const opengl_render_system&) = delete;

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

    private:

        /** Collection of entities in a scene to render. */
        std::vector<std::shared_ptr<entity>> scene_;

        /** Camera to render scene through */
        std::shared_ptr<camera> camera_;

        /** Global material. */
        std::unique_ptr<gl::material> material_;
};

}

