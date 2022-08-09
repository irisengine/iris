////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <vector>

#include "core/matrix4.h"
#include "core/transform.h"
#include "graphics/mesh.h"
#include "graphics/render_entity.h"
#include "graphics/render_entity_type.h"

namespace iris
{

/**
 * Implementation of RenderEntity for an instanced mesh i.e. rendering multiple copies of a single mesh with a single
 * draw call.
 *
 * Note that instances are static and cannot be modified after creation.
 */
class InstancedEntity : public RenderEntity
{
  public:
    /**
     * Construct a new InstancedEntity object.
     *
     * @param mesh
     *   Mesh to render.
     *
     * @param instances
     *   Collection of transforms describing each instance, must be greater that one.
     */
    InstancedEntity(const Mesh *mesh, const std::vector<Transform> &instances);
    ~InstancedEntity() override = default;

    /**
     * Get entity type.
     *
     * @returns
     *   Entity type.
     */
    RenderEntityType type() const override;

    /**
     * Get if entity will be rendered with transparency.
     *
     * @returns
     *   True if entity will have transparency, otherwise false.
     */
    bool has_transparency() const override;

    /**
     * Get number of instances of mesh to render.
     *
     * @returns
     *   Number of instances.
     */
    std::size_t instance_count() const;

    /**
     * Get collection of Matrix4 objects for all instances. For each instance the transform and normal transform is
     * returned.
     *
     * @return
     *   [transform , normal transform] for each instance.
     */
    const std::vector<Matrix4> &data() const;

  private:
    /** Number of instances to render. */
    std::size_t instance_count_;

    /** Render data for instances. */
    std::vector<Matrix4> data_;
};

}