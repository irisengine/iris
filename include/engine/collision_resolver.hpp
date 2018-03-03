#pragma once

#include <vector>

#include "contact.hpp"

namespace eng::collision
{

/**
 * Resolve all collisions for a supplied set of contacts. Each contact object
 * contains pointers to the colliding rigid bodies, which will be manipulated
 * by this function.
 *
 * @param contacts
 *   Collection of collision contact objects.
 */
void resolve(std::vector<contact> &contacts);

}

