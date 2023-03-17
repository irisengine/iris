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

#include "graphics/lights/light_type.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/material.h"
#include "graphics/primitive_type.h"
#include "graphics/render_graph/render_graph.h"

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
     * @param input_descriptors
     *   D3D12 vertex descriptor describing how to organise vertex data.
     *
     * @param light_type
     *   Type of light for this material.
     *
     * @param root_signature
     *   The root signature to use for the shaders.
     *
     * @param render_to_swapchain
     *   True if material will be rendered to the swapchain, false otherwise (render target).
     *
     * @param render_to_normal_target
     *   Flag indicating whether the material should also write out screen space normals to a render texture.
     *
     * @param render_to_position_target
     *   Flag indicating whether the material should also write out screen space positions to a render texture.
     *
     * @param has_transparency
     *   Hint to the renderer that the material will contain transparency.
     *
     * @param wireframe
     *   Flag indicating if wireframe should be rendered.
     */
    D3D12Material(
        const RenderGraph *render_graph,
        const std::vector<D3D12_INPUT_ELEMENT_DESC> &input_descriptors,
        PrimitiveType primitive_type,
        LightType light_type,
        ID3D12RootSignature *root_signature,
        bool render_to_swapchain,
        bool render_to_normal_target,
        bool render_to_position_target,
        bool has_transparency,
        bool wireframe);

    ~D3D12Material() override = default;

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
};

}
