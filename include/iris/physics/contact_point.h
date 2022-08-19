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
 * Struct encapsulating data that describes a rigid body contact.
 */
struct ContactPoint
{
    /** Rigid body in contact. */
    RigidBody *contact;

    /** Position in world space of contact. */
    Vector3 position;

    /** Amount contact is penetrating. */
    float penetration;

    /** Normal of contact in world space. */
    Vector3 normal;
};

}
