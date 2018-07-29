#pragma once

#include <memory>

#include "entity.hpp"
#include "texture.hpp"
#include "vector3.hpp"

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
class sprite final
{
    public:

        /**
         * Construct a sprite with an existing entity. This will cause the
         * entoty to be rendered as a sprite.
         *
         * @param e
         *   Entity to render.
         */
        sprite(std::shared_ptr<entity> e);

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
         * Create a sprite with a texture.
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
        sprite(
            const float x,
            const float y,
            const float width,
            const float height,
            const vector3 &colour,
            texture &&tex);

        /** Default */
        ~sprite() = default;
        sprite(sprite&&) = default;
        sprite& operator=(sprite&) = default;

        /** Disabled */
        sprite(const sprite&) = delete;
        sprite& operator=(const sprite&) = delete;

        /**
         * Get a shared_ptr to the renderable object of the sprite.
         *
         * @param
         *   shared_ptr to underlying entity.
         */
        std::shared_ptr<entity> renderable() const;

    private:

        /** Underlying entity for sprite. */
        std::shared_ptr<entity> entity_;

};

}

