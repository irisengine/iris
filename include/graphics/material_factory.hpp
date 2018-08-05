#pragma once

#include <memory>

#include "material.hpp"

namespace eng::material_factory
{

/**
 * Create a material for rendering a basic mesh.
 */
std::shared_ptr<material> basic_mesh();

/**
 * Create a material for rendering a basic sprite.
 */
std::shared_ptr<material> basic_sprite();

}


