#pragma once

#include "mesh.hpp"
#include "texture.hpp"
#include "vector3.hpp"

namespace eng::shape_factory
{

/**
 * Create a sprite mesh.
*/
mesh make_sprite(
    const vector3 &colour,
    texture &&tex);
}

