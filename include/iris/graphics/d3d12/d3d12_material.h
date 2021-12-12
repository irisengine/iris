////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "graphics/cube_map.h"
#include "graphics/lights/light_type.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/material.h"
#include "graphics/primitive_type.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/texture.h"

namespace iris
{

/**
 * Implementation of Material for d3d12.
 */
class D3D12Material : public Material
{
  public:
    /**
     * Construct a new D3D12Material.
     *
     * @param render_graph
     *   RenderGraph describing material.
     *
     * @param mesh
     *   Mesh material will be applied to.
     *
     * @param input_descriptors
     *   D3D12 vertex descriptor describing how to organise vertex data.
     *
     * @param light_type
     *   Type of light for this material.
     *
     * @param render_to_swapchain
     *   True if material will be rendered to the swapchain, false otherwise
     *   (render target).
     */
    D3D12Material(
        const RenderGraph *render_graph,
        const std::vector<D3D12_INPUT_ELEMENT_DESC> &input_descriptors,
        PrimitiveType primitive_type,
        LightType light_type,
        bool render_to_swapchain);

    ~D3D12Material() override = default;

    /**
     * Get Textures used in this material.
     *
     * @returns
     *   Textures used.
     */
    std::vector<Texture *> textures() const override;

    /**
     * Get the CubeMap used by this Material (if any).
     *
     * @returns
     *   CuveMap, or nullptr if not used by Material.
     */
    const CubeMap *cube_map() const override;

    /**
     * Get the d3d12 pipeline state for this material.
     *
     * @returns
     *   Pipeline state.
     */
    ID3D12PipelineState *pso() const;

  private:
    /** Pipeline state object. */
    ::Microsoft::WRL::ComPtr<ID3D12PipelineState> pso_;

    /** Collection of textures used. */
    std::vector<Texture *> textures_;

    /** Optional CubeMap for material. */
    const CubeMap *cube_map_;
};

}
