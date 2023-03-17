////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>
#include <cstddef>
#include <span>
#include <vector>

#include "graphics/lights/light_type.h"

namespace iris
{

class Material;
class RenderGraph;
class RenderEntity;

/**
 * Abstract class for creating and managing materials.
 *
 * This class also manages property buffers. These are byte buffers into which user values are copied and then uploaded
 * to the gpu. This allows runtime changing of variables in shaders.
 */
class MaterialManager
{
  public:
    MaterialManager() = default;
    virtual ~MaterialManager() = default;

    MaterialManager(const MaterialManager &) = delete;
    MaterialManager &operator=(const MaterialManager &) = delete;
    MaterialManager(MaterialManager &&) = default;
    MaterialManager &operator=(MaterialManager &&) = default;

    /**
     * Create a new material. Implementations should use a cache to avoid recreating the same material.
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
     * @param has_transparency
     *   Hint to the renderer that the material will contain transparency.
     *
     * @returns
     *   Pointer to created Material.
     */
    virtual Material *create(
        RenderGraph *render_graph,
        RenderEntity *render_entity,
        LightType light_type,
        bool render_to_colour_target,
        bool render_to_normal_target,
        bool render_to_position_target,
        bool has_transparency) = 0;

    /**
     * Clear all cached materials. This will invalidate any returned pointers.
     */
    virtual void clear() = 0;

    /**
     * Get property buffer for a render graph.
     *
     * @returns
     *   A span to the allocated buffer.
     */
    std::span<std::byte> create_property_buffer();

  private:
    /** Collection of property buffers. */
    std::vector<std::array<std::byte, 256u>> property_buffers_;
};

}
