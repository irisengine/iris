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

std::vector<Mesh> load(const std::string &mesh_file);

}
