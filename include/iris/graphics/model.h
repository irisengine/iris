#pragma once

#include "core/vector3.h"
#include "graphics/mesh.h"
#include "graphics/render_entity.h"
#include "graphics/skeleton.h"

namespace iris
{

/**
 * An implementation of RenderEntity that draws a mesh.
 */
class Model : public RenderEntity
{
  public:
    /**
     * Create a model.
     *
     * @param position
     *   Centre of mesh in world coordinates.
     *
     * @param scale
     *   Scale of mesh.
     *
     * @param mesh
     *   Mesh data.
     */
    Model(
        const Vector3 &position,
        const Vector3 &scale,
        std::vector<Mesh> meshes);

    /**
     * Create a model.
     *
     * @param position
     *   Centre of mesh in world coordinates.
     *
     * @param scale
     *   Scale of mesh.
     *
     * @param mesh
     *   Mesh data.
     *
     * @param skeleton
     *   Skeleton.
     */
    Model(
        const Vector3 &position,
        const Vector3 &scale,
        std::vector<Mesh> meshes,
        Skeleton skeleton);

    // default
    ~Model() override = default;
};

}
