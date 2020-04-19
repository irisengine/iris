#pragma once

#include <vector>

#include "core/matrix4.h"
#include "core/quaternion.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"

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
class Sprite
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
        Sprite(
            const float x,
            const float y,
            const float width,
            const float height,
            const Vector3 &colour);

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
         * @param colour
         *   Colour of sprite.
         *
         * @param tex
         *   Texture of sprite.
         */
        Sprite(
            const float x,
            const float y,
            const float width,
            const float height,
            const Vector3 &colour,
            Texture &&tex);

        /** Disabled */
        Sprite(const Sprite&) = delete;
        Sprite& operator=(const Sprite&) = delete;

        /**
         * Set the position of the sprite.
         *
         * @param position
         *   New position.
         */
        void set_position(const Vector3 &position);

        /**
         * Set the orientation of the matrix.
         *
         * @param orientation
         *   New oritentation.
         */
        void set_orientation(const Quaternion &orientation);

        /**
         * Get the transformation matrix of the sprite.
         *
         * @returns
         *   Transformation matrix.
         */
        Matrix4 transform() const;

        /**
         * Get a const reference to the Mesh that make up the sprite.
         *
         * @returns
         *   Const reference to mesh.
         */
        const Mesh& mesh() const;

        /**
         * Get a const reference to the sprites rendering material.
         *
         * @returns
         *   Const reference to material.
         */
        const Material& material() const;

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
        Mesh mesh_;

        /** The position of the sprite. */
        Vector3 position_;

        /** The orientation of the sprite. */
        Quaternion orientation_;

        /** The scale of the sprite. */
        Vector3 scale_;

        /** Model transformation matrix4. */
        Matrix4 model_;

        /** Material to render with. */
        Material* material_;

        /** Whether the object should be rendered as a wireframe. */
        bool wireframe_;
};

}

