#pragma once

#include "core/vector3.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"

namespace eng::shape_factory
{

/**
 * Create a Sprite mesh.
*/
Mesh sprite(
    const Vector3 &colour,
    Texture &&tex);
}

