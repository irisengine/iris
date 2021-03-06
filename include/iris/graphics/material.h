#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>

#include "graphics/lights/lighting_rig.h"
#include "graphics/mesh.h"
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
    /**
     * Construct a new material.
     *
     * @param render_graph
     *   RenderGraph describing material.
     *
     * @param mesh
     *   Mesh.
     *
     * @param lighting_rig
     *   Lights that will effect this material.
     */
    Material(
        const RenderGraph *render_graph,
        const Mesh &mesh,
        const LightingRig *lighting_rig);

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
