#pragma once

#include <vector>

#include "core/matrix4.hpp"
#include "core/quaternion.hpp"
#include "graphics/material.hpp"
#include "graphics/mesh.hpp"
#include "graphics/texture.hpp"

namespace eng
{

/**
 * This class encapsulates an entity that can only be rendered in a 2d plane
 * facing this screen. Positions are done in terms of screen units i.e.
 *
 *   -1, 1  +---------+ 1, 1
 *          |         |
 *          |         |
 *          |    *    |
 *          |         |
 *   -1, -1 +---------+ -1, -1
 *
 *   With the centre (*) being 0, 0
 */
class sprite
{
    public:

        /**
         * Create a solid colour sprite.
         *
         * @param x
         *   Screen x coordinate of centre of sprite.
         *
         * @param y
         *   Screen y coordinate of centre of sprite.
         *
         * @param width
         *   Width of sprite.
         *
         * @param height
         *   Height of sprite.
         *
         * @param colour
         *   Colour of sprite.
         */
        sprite(
            const float x,
            const float y,
            const float width,
            const float height,
            const vector3 &colour);

        /**
         * Create a textures sprite.
         *
         * @param x
         *   Screen x coordinate of centre of sprite.
         *
         * @param y
         *   Screen y coordinate of centre of sprite.
         *
         * @param width
         *   Width of sprite.
         *
         * @param height
         *   Height of sprite.
         *
         * @param tex
         *   Texture of sprite.
         *
         * @param colour
         *   Colour of sprite.
         */
        sprite(
            const float x,
            const float y,
            const float width,
            const float height,
            const vector3 &colour,
            texture &&tex);

        /** Disabled */
        sprite(const sprite&) = delete;
        sprite& operator=(const sprite&) = delete;

        /**
         * Set the position of the sprite.
         *
         * @param position
         *   New position.
         */
        void set_position(const vector3 &position);

        /**
         * Set the orientation of the matrix.
         *
         * @param orientation
         *   New oritentation.
         */
        void set_orientation(const quaternion &orientation);

        /**
         * Get the transformation matrix of the sprite.
         *
         * @returns
         *   Transformation matrix.
         */
        matrix4 transform() const;

        /**
         * Get a const reference to the mesh that make up the sprite.
         *
         * @returns
         *   Const reference to mesh.
         */
        const mesh& render_mesh() const;

        /**
         * Get a const reference to the sprites rendering material.
         *
         * @returns
         *   Const reference to material.
         */
        const material& mat() const;

        /**
         * Returns whether the object should be rendered as a wireframe.
         *
         * @returns
         *   True if should be rendered as a wireframe, false otherwise.
         */
        bool should_render_wireframe() const;

        /**
         * Sets whether the object should be rendered as a wireframe.
         *
         * @param wrireframe
         *   True if should be rendered as a wireframe, false otherwise.
         */
        void set_wireframe(const bool wireframe);

    private:

        /** Collection of meshes. */
        mesh mesh_;

        /** The position of the sprite. */
        vector3 position_;

        /** The orientation of the sprite. */
        quaternion orientation_;

        /** The scale of the sprite. */
        vector3 scale_;

        /** Model transformation matrix4. */
        matrix4 model_;

        /** Material to render with. */
        material* material_;

        /** Whether the object should be rendered as a wireframe. */
        bool wireframe_;
};

}

