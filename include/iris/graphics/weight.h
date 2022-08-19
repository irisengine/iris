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
    /** Vertex index.*/
    std::uint32_t vertex;

    /** Influence. */
    float weight;

    /** Name of bone weight is for. */
    std::string bone_name;
};

}
