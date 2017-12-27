#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include "camera.hpp"
#include "gl/entity_data.hpp"
#include "matrix.hpp"
#include "mesh.hpp"

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
         * Add a mesh to the scene.
         *
         * @param m
         *   Mesh to add.
         */
        void add(std::shared_ptr<mesh> m);

        /**
         * Render the current scene.
         */
        void render() const;

    private:

        /**
         * Convenient alias for type consisting of a mesh and opengl rendering
         * data.
         * */
        using entity = std::tuple<std::shared_ptr<mesh>, gl::entity_data>;

        /** Collection of entities in a scene to render. */
        std::vector<entity> scene_;

        /** Projection matrix. */
        matrix projection_;

        /* View matrix */
        matrix view_;
};

}

