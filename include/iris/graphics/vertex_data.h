////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/colour.h"
#include "core/vector3.h"
#include "graphics/bone.h"
#include "graphics/vertex_attributes.h"

namespace iris
{

/**
 * Struct encapsulating all data needed to render a vertex. This automatically
 * pads fields required by some graphics APIs.
 */
struct VertexData
{
    VertexData() = default;

    /**
     * Create a new VertexData.
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
    VertexData(const Vector3 &position, const Vector3 &normal, const Colour &colour, const Vector3 &texture_coords)
        : VertexData(
              position,
              normal,
              colour,
              texture_coords,
              {},
              {},
              {{{0u, 1.0f}, {0u, 0.0f}, {0u, 0.0f}, {0u, 0.0f}}})
    {
    }

    VertexData(
        const Vector3 &position,
        const Vector3 &normal,
        const Colour &colour,
        const Vector3 &texture_coords,
        const Vector3 &tangent,
        const Vector3 &bitangent)
        : VertexData(
              position,
              normal,
              colour,
              texture_coords,
              tangent,
              bitangent,
              {{{0u, 1.0f}, {0u, 0.0f}, {0u, 0.0f}, {0u, 0.0f}}})
    {
    }

    VertexData(
        const Vector3 &position,
        const Vector3 &normal,
        const Colour &colour,
        const Vector3 &texture_coords,
        const Vector3 &tangent,
        const Vector3 &bitangent,
        std::array<Weight, 4u> weights)
        : position(position)
        , pos_w(1.0f)
        , normal(normal)
        , normal_w(0.0f)
        , colour(colour)
        , texture_coords(texture_coords)
        , padding(1.0f)
        , tangent(tangent)
        , tangent_w(0.0f)
        , bitangent(bitangent)
        , bitangent_w(0.0f)
        , bone_ids({})
        , bone_weights({})
    {
        for (auto i = 0u; i < weights.size(); ++i)
        {
            bone_ids[i] = weights[i].vertex;
            bone_weights[i] = weights[i].weight;
        }
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
    Colour colour;

    /** Texture coordinates. */
    Vector3 texture_coords;

    /** Padding so we can pass normal as 4 floats. */
    float padding;

    /** Normal tangent. */
    Vector3 tangent;

    /** Padding so we can pass normal as 4 floats. */
    float tangent_w;

    /** Normal bitangent tangent. */
    Vector3 bitangent;

    /** Padding so we can pass normal as 4 floats. */
    float bitangent_w;

    /** Array of bone ids. */
    std::array<std::uint32_t, 4u> bone_ids;

    /** Array of bone weights. */
    std::array<float, 4u> bone_weights;
};

/**
 * VertexAttributes for above struct.
 */
static VertexAttributes DefaultVertexAttributes{
    {VertexAttributeType::FLOAT_4,
     VertexAttributeType::FLOAT_4,
     VertexAttributeType::FLOAT_4,
     VertexAttributeType::FLOAT_4,
     VertexAttributeType::FLOAT_4,
     VertexAttributeType::FLOAT_4,
     VertexAttributeType::UINT32_4,
     VertexAttributeType::FLOAT_4}};

}
