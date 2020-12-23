#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>

#include "graphics/buffer_descriptor.h"
#include "graphics/light.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/texture.h"

namespace iris
{

/**
 * Class encapsulating a material for rendering an entity.
 */
class Material
{
  public:
    Material(
        /**
         * Construct a new material.
         *
         * @param render_graph
         *   RenderGraph describing material.
         *
         * @param vertex_descriptor
         *   BufferDescriptor for vertex data.
         *
         * @param lights
         *   Collection of lights that will effect this material.
         */
        const RenderGraph &render_graph,
        const BufferDescriptor &vertex_desciptor,
        const std::vector<Light *> &lights);

    ~Material();
    Material(Material &&);
    Material &operator=(Material &&);

    /**
     * Get a native handle for the material. The type of this is dependent
     * on the current graphics API.
     *
     * @returns
     *   Graphics API specific handle.
     */
    std::any native_handle() const;

    /**
     * Get Textures used in this material.
     *
     * @returns
     *   Textures used.
     */
    std::vector<Texture *> textures() const;

  private:
    /** Collection of textures used. */
    std::vector<Texture *> textures_;

    /** Graphics API implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
