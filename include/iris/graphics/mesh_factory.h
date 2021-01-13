#pragma once

#include <tuple>

#include "core/colour.h"
#include "core/vector3.h"
#include "graphics/buffer_descriptor.h"
#include "graphics/skeleton.h"
#include "graphics/texture.h"

namespace iris::mesh_factory
{

/**
 * Create an empty mesh.
 *
 * @returns
 *   BufferDescriptor for empty object.
 */
BufferDescriptor empty();

/**
 * Create a Sprite mesh.
 *
 * @param colour
 *   Colour of sprite.
 *
 * @returns
 *   BufferDescriptor for sprite.
 */
BufferDescriptor sprite(const Colour &colour);

/**
 * Create a cube mesh.
 *
 * @param colour
 *   Colour of cube.
 *
 * @returns
 *   BufferDescriptor for cube.
 */
BufferDescriptor cube(const Colour &colour);

/**
 * Create a plane mesh.
 *
 * @param colour
 *   Colour of plane.
 *
 * @param divisions
 *   Number of divisions (both horizontal and vertical).
 *
 * @returns
 *   BufferDescriptor for cube.
 */
BufferDescriptor plane(const Colour &colour, std::uint32_t divisions);

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
 *   BufferDescriptor for sprite.
 */
BufferDescriptor quad(
    const Colour &colour,
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
 *   BufferDescriptor for lines.
 */
BufferDescriptor lines(
    const std::vector<Vector3> &line_data,
    const Colour &colour);

/**
 * Create a mesh for a collection of lines.
 *
 * @param line_data
 *   Collection of data points representing (potentially disjoint) lines to
 * . render, the tuple contains:
 *   [start_position, start_colour, end_position, end_colour]
 *
 * @returns
 *   BufferDescriptor for lines.
 */
BufferDescriptor lines(
    const std::vector<std::tuple<Vector3, Colour, Vector3, Colour>> &line_data);

/**
 * Load a mesh from file.
 *
 * @param mesh_file
 *   File to load.
 *
 * @returns
 *   Tuple of <collection of buffer descriptors, skeleton>. If the animation
 *   file did not contain skeletal data then the skeleton object will be a
 *   default one.
 */
std::tuple<std::vector<BufferDescriptor>, Skeleton> load(
    const std::string &mesh_file);

}
