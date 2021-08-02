#pragma once

#include <vector>

#include "graphics/texture.h"

namespace iris
{

/**
 * Interface for a Material - a class which which encapsulates how to render
 * a Mesh.
 *
 * This interface is deliberately limited, most of the functionality is provided
 * by the implementations, which in turn is only used internally by the engine.
 */
class Material
{
  public:
    virtual ~Material() = default;

    /**
     * Get all the textures used by this Material
     *
     * @returns
     *   Collection of Texture objects.
     */
    virtual std::vector<Texture *> textures() const = 0;
};

}
