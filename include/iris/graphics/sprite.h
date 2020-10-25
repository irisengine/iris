#pragma once

#include "core/real.h"
#include "core/vector3.h"
#include "graphics/material.h"
#include "graphics/render_entity.h"
#include "graphics/texture.h"

namespace iris
{

/**
 * An implementation of RenderEntity that draws a sprite (i.e. a quad) that
 * always faces the screen and is relative to the screen coordinates.
 */
class Sprite : public RenderEntity
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
    Sprite(real x, real y, real width, real height, const Vector3 &colour);

    /**
     * Create a textured sprite.
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
        real x,
        real y,
        real width,
        real height,
        const Vector3 &colour,
        Texture *tex);

    // default
    ~Sprite() override = default;

    /**
     * Set sprite texture.
     *
     * @param texture
     *   New sprite texture.
     */
    void set_texture(Texture *texture);

  private:
    /** Colour of sprite. */
    Vector3 colour_;
};

}
