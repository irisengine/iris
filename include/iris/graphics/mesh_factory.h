#pragma once

#include "core/vector3.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"

namespace iris::mesh_factory
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
std::vector<Mesh> sprite(const Vector3 &colour, Texture &&texture);

/**
 * Create a cube mesh.
 *
 * @param colour
 *   Colour of cube.
 *
 * @returns
 *   Mesh for cube.
 */
std::vector<Mesh> cube(const Vector3 colour);

/**
 * Create a Quad mesh.
 *
 * @param colour
 *   Colour of quad.
 *
 * @param lower_left
 *   World coords of lower left of quad.
 *
 * @param lower_right
 *   World coords of lower right of quad.
 *
 * @param upper_left
 *   World coords of upper left of quad.
 *
 * @param upper_right
 *   World coords of upper right of quad.
 *
 * @returns
 *   Mesh for sprite.
 */
std::vector<Mesh> quad(
    const Vector3 &colour,
    const Vector3 &lower_left,
    const Vector3 &lower_right,
    const Vector3 &upper_left,
    const Vector3 &upper_right);

std::vector<Mesh> load(const std::string &mesh_file);

}
