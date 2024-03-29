////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_material.h"

#include <any>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <d3dcompiler.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/error_handling.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_root_signature.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/render_graph/shader_compiler.h"
#include "graphics/shader_type.h"
#include "log/log.h"

#pragma comment(lib, "d3dcompiler.lib")

namespace
{
/**
 * Helper function to create a d3d12 shader.
 *
 * @param source
 *   Shader source.
 *
 * @param type
 *   Type of shader.
 *
 * @returns
 *   D3D12 handle to created shader.
 */
Microsoft::WRL::ComPtr<ID3DBlob> create_shader(const std::string &source, iris::ShaderType type)
{
    const auto target = type == iris::ShaderType::VERTEX ? "vs_5_1" : "ps_5_1";

    Microsoft::WRL::ComPtr<ID3DBlob> shader = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> error = nullptr;
    if (::D3DCompile(
            source.c_str(),
            source.length(),
            NULL,
            NULL,
            NULL,
            "main",
            target,
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES,
            0u,
            &shader,
            &error) != S_OK)
    {
        const std::string error_message(static_cast<char *>(error->GetBufferPointer()), error->GetBufferSize());

        auto line_number = 1u;

        std::istringstream strm{source};
        for (std::string line; std::getline(strm, line);)
        {
            LOG_ENGINE_ERROR("d3d12_material", "{}: {}", line_number, line);
            ++line_number;
        }

        LOG_ENGINE_ERROR("d3d12_material", "{}", error_message);

        throw iris::Exception("shader compile failed: " + error_message);
    }

    LOG_ENGINE_INFO("d3d12_material", "shader created");

    return shader;
}

}

namespace iris
{

D3D12Material::D3D12Material(
    const RenderGraph *render_graph,
    const std::vector<D3D12_INPUT_ELEMENT_DESC> &input_descriptors,
    PrimitiveType primitive_type,
    LightType light_type,
    ID3D12RootSignature *root_signature,
    bool render_to_swapchain,
    bool render_to_normal_target,
    bool render_to_position_target,
    bool has_transparency,
    bool wireframe)
    : Material(render_graph)
    , pso_()
{
    ShaderCompiler compiler{
        ShaderLanguage::HLSL, render_graph, light_type, render_to_normal_target, render_to_position_target};
    const auto vertex_source = compiler.vertex_shader();
    const auto fragment_source = compiler.fragment_shader();

    const auto vertex_shader = create_shader(vertex_source, ShaderType::VERTEX);
    const auto fragment_shader = create_shader(fragment_source, ShaderType::FRAGMENT);

    auto *device = D3D12Context::device();

    // setup various descriptors for pipeline state

    auto blend_state = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    blend_state.RenderTarget[0].BlendEnable = FALSE;

    if (light_type == LightType::AMBIENT)
    {
        if (has_transparency)
        {
            blend_state.RenderTarget[0].BlendEnable = TRUE;
            blend_state.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            blend_state.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
            blend_state.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        }
    }
    else
    {
        blend_state.RenderTarget[0].BlendEnable = TRUE;
        blend_state.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
        blend_state.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    }

    auto depth_state = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    depth_state.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    depth_state.DepthWriteMask =
        (light_type == LightType::AMBIENT) ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;

    D3D12_RASTERIZER_DESC rasterizer_description = {0};
    rasterizer_description.FillMode = wireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
    rasterizer_description.CullMode = D3D12_CULL_MODE_NONE;
    rasterizer_description.FrontCounterClockwise = TRUE;
    rasterizer_description.DepthClipEnable = TRUE;
    rasterizer_description.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC descriptor = {};
    descriptor.InputLayout = {input_descriptors.data(), static_cast<UINT>(input_descriptors.size())};
    descriptor.pRootSignature = root_signature;
    descriptor.VS = CD3DX12_SHADER_BYTECODE(vertex_shader.Get());
    descriptor.PS = CD3DX12_SHADER_BYTECODE(fragment_shader.Get());
    descriptor.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    descriptor.BlendState = blend_state;
    descriptor.DepthStencilState = depth_state;
    descriptor.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descriptor.SampleMask = UINT_MAX;
    descriptor.RasterizerState = rasterizer_description;
    descriptor.SampleDesc.Count = 1;

    // set formats for all render targets we might use, it's ok to set these even if they are unused
    descriptor.NumRenderTargets = 3;
    descriptor.RTVFormats[0] = !render_to_swapchain ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;
    descriptor.RTVFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    descriptor.RTVFormats[2] = DXGI_FORMAT_R16G16B16A16_FLOAT;

    switch (primitive_type)
    {
        case PrimitiveType::TRIANGLES: descriptor.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; break;
        case PrimitiveType::LINES: descriptor.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; break;
    }

    // create pipeline state
    expect(device->CreateGraphicsPipelineState(&descriptor, IID_PPV_ARGS(&pso_)) == S_OK, "could not create pso");

    static int counter = 0;
    std::wstringstream strm{};
    strm << L"pso_" << counter++;
    const auto name = strm.str();
    pso_->SetName(name.c_str());
}

ID3D12PipelineState *D3D12Material::pso() const
{
    return pso_.Get();
}

}
