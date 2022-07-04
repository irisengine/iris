////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graphics/d3d12/d3d12_material.h"
#include "graphics/lights/light_type.h"
#include "graphics/material_cache.h"
#include "graphics/material_manager.h"

namespace iris
{

class Material;
class RenderGraph;
class RenderTarget;

/**
 * Implementation of MaterialManager for D3D12.
 */
class D3D12MaterialManager : public MaterialManager
{
  public:
    ~D3D12MaterialManager() override = default;

    /**
     * Create a new material.
     *
     * @param render_graph
     *   RenderGraph describing material.
     *
     * @param render_entity
     *   The entity material is for.
     *
     * @param light_type
     *   The type of light that material should use.
     *
     * @param render_to_colour_target
     *   Whether the material is rendering to a colour target or the back buffer.
     *
     * @param render_to_normal_target
     *   Whether to render screen space normals.
     *
     * @param render_to_position_target.
     *   Whether to render screen positions.
     *
     * @returns
     *   Pointer to created Material.
     */
    Material *create(
        RenderGraph *render_graph,
        RenderEntity *render_entity,
        LightType light_type,
        bool render_to_colour_target,
        bool render_to_normal_target,
        bool render_to_position_target);

    /**
     * Clear all cached materials. This will invalidate any returned pointers.
     */
    void clear() override;

  private:
    /** Cache of created materials. */
    MaterialCache<D3D12Material, RenderGraph *, LightType, bool, bool> materials_;
};

}
