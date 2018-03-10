#pragma once

#include <vector>

#include "contact.hpp"
#include "rigid_body.hpp"

namespace eng::collision
{

/**
 * Get a collection of contact objects for each penetration point between two
 * rigid bodies, if they are colliding.
 *
 * This function guarantees that for each resulting contact struct, if one of
 * the bodies is static then the body2 field points to that.
 *
 * @param body1
 *   The first rigid body to test for collision.
 *
 * @param body2
 *   The second rigid body to test for collision.
 *
 * @returns
 *   A collection of contact objects for each penetration point between both
 *   supplied bodies, or an empty collection if they do not intersect.
 */
std::vector<contact> detect(rigid_body &body1, rigid_body &body2);

}

