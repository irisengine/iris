#pragma once

#include <any>
#include <cstdint>
#include <memory>
#include <vector>

#include "core/matrix4.h"
#include "core/vector3.h"
#include "graphics/buffer.h"
#include "graphics/buffer_descriptor.h"
#include "graphics/texture.h"
#include "graphics/vertex_data.h"

namespace iris
{

/**
 * This class encapsulates data that represents a renderable mesh. This includes
 * buffer descriptors and textures. By itself it is just a container, it should
 * be passed to a RenderEntity which handles the remaining data and logic to
 * actually render.
 */
class Mesh
{
  public:
    /**
     * Construct a new Mesh with no texture (a default blank texture will be
     * used).
     *
     * @param buffer_descriptor
     *   Descriptor for mesh vertex data.
     */
    Mesh(BufferDescriptor buffer_descriptor);

    /**
     * Construct a new Mesh.
     *
     * @param buffer_descriptor
     *   Descriptor for mesh vertex data.
     *
     * @param texture
     *   Texture for mesh.
     */
    Mesh(BufferDescriptor buffer_descriptor, Texture *texture);

    // default
    ~Mesh() = default;
    Mesh(Mesh &&) = default;
    Mesh &operator=(Mesh &&) = default;

    /**
     * Get a reference to the buffer descriptor.
     *
     * @returns
     *   Const reference to buffer descriptor.
     */
    const BufferDescriptor &buffer_descriptor() const;

    /**
     * Get a reference to the Texture for this mesh.
     *
     * @returns
     *   Const reference to texture.
     */
    const Texture *texture() const;

  private:
    /** Descriptor for vertex data. */
    BufferDescriptor buffer_descriptor_;

    /** Texture to render Mesh with. */
    Texture *texture_;
};

}
