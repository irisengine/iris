////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

namespace iris
{

/**
 * A struct encapsulating how much influence a bone has over a vertex (i.e. it's
 * weight).
 */
struct Weight
{
    /**
     * Construct a new weight.
     *
     * @param vertex
     *   Index of vertex this weight applies to.
     *
     * @param weight
     *   Influence over vertex, must be in the range [0.0, 1.0].
     */
    Weight(std::uint32_t vertex, float weight)
        : vertex(vertex)
        , weight(weight)
    {
    }

    /** Vertex index.*/
    std::uint32_t vertex;

    /** Influence. */
    float weight;
};

}
