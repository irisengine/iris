#pragma once

#include <memory>

#include "material.h"

namespace iris::material_factory
{

/**
 * Create a Material for rendering a basic sprite.
 *
 * @returns
 *   Sprite material.
 */
Material* sprite();

/**
 * Create a Material for rendering a basic mesh.
 *
 * @returns
 *   Mesh material.
 */
Material* mesh();

}


