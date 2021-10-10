////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#import <Metal/Metal.h>

#include "graphics/lights/light_type.h"
#include "graphics/material.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/texture.h"

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
     */
    MetalMaterial(
        const RenderGraph *render_graph,
        MTLVertexDescriptor *descriptors,
        LightType light_type);

    ~MetalMaterial() override = default;

    /**
     * Get all textures used by this material.
     *
     * @returns
     *   Collection of Texture objects used by this material.
     */
    std::vector<Texture *> textures() const override;

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

    /** Collection of Texture objects used by material. */
    std::vector<Texture *> textures_;
};

}
