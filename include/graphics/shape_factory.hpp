#pragma once

#include "core/vector3.hpp"
#include "graphics/mesh.hpp"
#include "graphics/texture.hpp"

namespace eng::shape_factory
{

/**
 * Create a sprite mesh.
*/
mesh make_sprite(
    const vector3 &colour,
    texture &&tex);
}

