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
 * Struct encapsulating data that describes a contact between two rigid bodies.
 */
struct ContactPoint
{
    /** First rigid body in contact. */
    RigidBody *contact_a;

    /** Second rigid body in contact. */
    RigidBody *contact_b;

    /** Position in world space of contact. */
    Vector3 position;
};

}
