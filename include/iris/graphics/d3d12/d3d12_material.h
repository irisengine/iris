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
     */
    D3D12Material(
        const RenderGraph *render_graph,
        const std::vector<D3D12_INPUT_ELEMENT_DESC> &input_descriptors,
        PrimitiveType primitive_type,
        LightType light_type);

    ~D3D12Material() override = default;

    /**
     * Get Textures used in this material.
     *
     * @returns
     *   Textures used.
     */
    std::vector<Texture *> textures() const override;

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
};

}
