#pragma once

#include <memory>
#include <vector>

#include "camera.hpp"
#include "entity.hpp"
#include "material.hpp"
#include "matrix.hpp"
#include "window.hpp"

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
         * @param w
         *   Rendering window.
         *
         * @param width
         *   Width of render window.
         *
         * @param height
         *   Height of render window.
         */
        render_system(
            std::shared_ptr<camera> c,
            std::shared_ptr<window> w,
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
         * Set the position of the one light in the scene.
         *
         * @param position.
         *   New position of light in world space.
         */
        void set_light_position(const vector3 &position) noexcept;

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

        /** Rendering window. */
        std::shared_ptr<window> window_;

        /** Global material. */
        material material_;

        /** Light position. */
        vector3 light_position;
};

}

