#pragma once

#include "vector3.hpp"

namespace eng
{

/**
 * Struct encapsulating all data needed to render a vertex.
 */
struct vertex_data
{
    /**
     * Create a new vertex_data.
     *
     * @param position
     *   The position of the vertex.
     *
     * @param texture_coords
     *   Coordinates of texture.
     */
    vertex_data(
        const vector3 &position,
        const vector3 &texture_coords)
        : position(position),
          texture_coords(texture_coords)
    { }

    /** Vertex position. */
    vector3 position;

    /** Texture coordinates. */
    vector3 texture_coords;
};

}

