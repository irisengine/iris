#pragma once

#include "core/vector3.h"

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
     * @param normal
     *   The normal of the vertex.
     *
     * @param colour
     *   Colour of the vertex.
     *
     * @param texture_coords
     *   Coordinates of texture.
     */
    vertex_data(
        const Vector3 &position,
        const Vector3 &normal,
        const Vector3 &colour,
        const Vector3 &texture_coords)
        : position(position),
          normal(normal),
          colour(colour),
          texture_coords(texture_coords)
    { }

    /** Vertex position. */
    Vector3 position;

    /** Vertex normal. */
    Vector3 normal;

    /** Vertex colour. */
    Vector3 colour;

    /** Texture coordinates. */
    Vector3 texture_coords;
};

}

