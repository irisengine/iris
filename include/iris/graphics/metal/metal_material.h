////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#include "graphics/lights/light_type.h"
#include "graphics/material.h"
#include "graphics/render_graph/render_graph.h"

namespace iris
{

/**
 * Implementation of Material for metal.
 */
class MetalMaterial : public Material
{
  public:
    /**
     * Construct a new MetalMaterial.
     *
     * @param render_graph
     *   RenderGraph that describes the material.
     *
     * @param descriptors
     *   Metal vertex descriptor describing how to organise vertex data.
     *
     * @param light_type
     *   Type of light for this material.
     *
     * @param render_to_normal_target
     *   Flag indicating whether the material should also write out screen space normals to a render texture.
     *
     * @param render_to_position_target
     *   Flag indicating whether the material should also write out screen space positions to a render texture.
     *
     * @param has_transparency
     *   Hint to the renderer that the material will contain transparency.
     */
    MetalMaterial(
        const RenderGraph *render_graph,
        MTLVertexDescriptor *descriptors,
        LightType light_type,
        bool render_to_normal_target,
        bool render_to_position_target,
        bool has_transparency);

    ~MetalMaterial() override = default;

    /**
     * Get the metal pipeline state for this material.
     *
     * @returns
     *   Pipeline state.
     */
    id<MTLRenderPipelineState> pipeline_state() const;

  private:
    /** Pipeline state object. */
    id<MTLRenderPipelineState> pipeline_state_;
};

}
