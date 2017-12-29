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
     */
    vertex_data(const vector3 &position)
        : position(position)
    { }

    /** Vertex position. */
    vector3 position;
};

}

