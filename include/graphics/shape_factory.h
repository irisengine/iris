#pragma once

#include "core/vector3.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"

namespace iris::shape_factory
{

/**
 * Create a Sprite mesh.
 *
 * @param colour
 *   Colour of sprite.
 *
 * @param texture
 *   Texture of sprite.
 *
 * @returns
 *   Mesh for sprite.
 */
Mesh sprite(const Vector3 &colour, Texture &&texture);

/**
 * Create a cube mesh.
 *
 * @param colour
 *   Colour of cube.
 *
 * @returns
 *   Mesh for cube.
 */
Mesh cube(const Vector3 colour);

}

