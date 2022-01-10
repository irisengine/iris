////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/vector3.h"

namespace iris
{

class RigidBody;

/**
 * Struct encapsulating a hit from a ray cast.
 */
struct RayCastResult
{
    /** Body hit. */
    RigidBody *body;

    /** Worlds space of ray intersection with body. */
    Vector3 position;
};

}
