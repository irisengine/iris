#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include "mesh.hpp"
#include "gl/entity_data.hpp"

namespace eng
{

/**
 * Class for rendering scenes with opengl.
 */
class opengl_render_system final
{
    public:

        /** Default */
        opengl_render_system() = default;
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
};

}

