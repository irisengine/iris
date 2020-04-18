#pragma once

#include "core/vector3.hpp"
#include "graphics/mesh.hpp"
#include "graphics/texture.hpp"

namespace eng::shape_factory
{

/**
 * Create a Sprite mesh.
*/
Mesh sprite(
    const Vector3 &colour,
    Texture &&tex);
}

