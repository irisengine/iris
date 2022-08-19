////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graphics/lights/light_type.h"
#include "graphics/material_cache.h"
#include "graphics/material_manager.h"
#include "graphics/opengl/opengl_material.h"

namespace iris
{

class Material;
class RenderGraph;
class RenderTarget;

/**
 * Implementation of MaterialManager for OpenGL.
 */
class OpenGLMaterialManager : public MaterialManager
{
  public:
    ~OpenGLMaterialManager() override = default;

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
     * @param has_transparency
     *   Hint to the renderer that the material will contain transparency.
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
        bool render_to_position_target,
        bool has_transparency);

    /**
     * Clear all cached materials. This will invalidate any returned pointers.
     */
    void clear() override;

  private:
    /** Cache of created materials. */
    MaterialCache<OpenGLMaterial, RenderGraph *, LightType, bool, bool> materials_;
};

}
