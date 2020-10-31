#pragma once

#include <tuple>

#include "core/vector3.h"
#include "graphics/mesh.h"
#include "graphics/skeleton.h"
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
std::vector<Mesh> sprite(const Vector3 &colour, Texture *texture);

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

/**
 * Create a mesh for a collection of lines.
 *
 * @param line_data
 *   Collection of points to draw lines between.
 *
 * @param colour
 *   Colour of lines.
 *
 * @returns
 *   Mesh for lines.
 */
std::vector<Mesh> lines(
    const std::vector<Vector3> &line_data,
    const Vector3 &colour);

/**
 * Create a mesh for a collection of lines.
 *
 * @param line_data
 *   Collection of data points representing (potentially disjoint) lines to
 * . render, the tuple contains:
 *   [start_position, start_colour, end_position, end_colour]
 *
 * @returns
 *   Mesh for lines.
 */
std::vector<Mesh> lines(
    const std::vector<std::tuple<Vector3, Vector3, Vector3, Vector3>>
        &line_data);

/**
 * Load a mesh from file.
 *
 * @param mesh_file
 *   File to load.
 *
 * @returns
 *   Tuple of <collection of meshes, skeleton>. If the animation file did not
 *   contain skeletal data then the skeleton object will be a default one.
 */
std::tuple<std::vector<Mesh>, Skeleton> load(const std::string &mesh_file);

}
