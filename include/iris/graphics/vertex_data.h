#pragma once

#include "core/vector3.h"
#include "graphics/vertex_attributes.h"

namespace iris
{

/**
 * Struct encapsulating all data needed to render a vertex. This automatically
 * pads fields required by some graphics APIs.
 */
struct vertex_data
{
    vertex_data() = default;

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
        : position(position)
        , pos_w(1.0f)
        , normal(normal)
        , normal_w(0.0f)
        , colour(colour)
        , colour_a(1.0f)
        , texture_coords(texture_coords)
        , padding(1.0f)
    {
    }

    /** Vertex position. */
    Vector3 position;

    /** w component so we can pass position as 4 floats. */
    float pos_w;

    /** Vertex normal. */
    Vector3 normal;

    /** w component so we can pass normal as 4 floats. */
    float normal_w;

    /** Vertex colour. */
    Vector3 colour;

    /** Alpha component so we can pass normal as 4 floats. */
    float colour_a;

    /** Texture coordinates. */
    Vector3 texture_coords;

    /** Padding so we can pass normal as 4 floats. */
    float padding;
};

/**
 * VertexAttributes for above struct.
 */
static VertexAttributes vertex_attributes{
    {VertexAttributeType::FLOAT_4,
     VertexAttributeType::FLOAT_4,
     VertexAttributeType::FLOAT_4,
     VertexAttributeType::FLOAT_4}};

}
