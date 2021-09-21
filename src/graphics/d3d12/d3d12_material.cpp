#include "graphics/d3d12/d3d12_material.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <d3dcompiler.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/exception.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/hlsl_shader_compiler.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/shader_type.h"

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
Microsoft::WRL::ComPtr<ID3DBlob> create_shader(
    const std::string &source,
    iris::ShaderType type)
{
    const auto target = type == iris::ShaderType::VERTEX ? "vs_5_0" : "ps_5_0";

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
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
            0u,
            &shader,
            &error) != S_OK)
    {
        const std::string error_message(
            static_cast<char *>(error->GetBufferPointer()),
            error->GetBufferSize());

        throw iris::Exception("shader compile failed: " + error_message);
    }

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
    std::uint32_t samples)
    : pso_()
    , textures_()
{
    if (samples == 0u)
    {
        samples = 1u;
    }

    HLSLShaderCompiler compiler{render_graph, light_type};
    const auto vertex_source = compiler.vertex_shader();
    const auto fragment_source = compiler.fragment_shader();

    const auto vertex_shader = create_shader(vertex_source, ShaderType::VERTEX);
    const auto fragment_shader =
        create_shader(fragment_source, ShaderType::FRAGMENT);

    textures_ = compiler.textures();

    auto *device = D3D12Context::device();
    auto *root_signature = D3D12Context::root_signature();

    // setup various descriptors for pipeline state

    auto blend_state = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    // set blend mode based on light
    // ambient is always rendered first (no blending)
    // directional and point are always rendered after (blending)
    blend_state.RenderTarget[0].BlendEnable = TRUE;
    blend_state.RenderTarget[0].DestBlend =
        (light_type == LightType::AMBIENT) ? D3D12_BLEND_ZERO : D3D12_BLEND_ONE;
    blend_state.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;

    auto depth_state = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    depth_state.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    D3D12_RASTERIZER_DESC rasterizer_description = {0};
    rasterizer_description.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizer_description.CullMode = D3D12_CULL_MODE_BACK;
    rasterizer_description.FrontCounterClockwise = TRUE;
    rasterizer_description.DepthClipEnable = TRUE;
    rasterizer_description.ConservativeRaster =
        D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC descriptor = {};
    descriptor.SampleDesc.Count = samples;

    if (samples > 1u)
    {
        rasterizer_description.MultisampleEnable = TRUE;
        descriptor.SampleDesc.Quality = 1u;
    }

    descriptor.InputLayout = {
        input_descriptors.data(), static_cast<UINT>(input_descriptors.size())};
    descriptor.pRootSignature = root_signature;
    descriptor.VS = CD3DX12_SHADER_BYTECODE(vertex_shader.Get());
    descriptor.PS = CD3DX12_SHADER_BYTECODE(fragment_shader.Get());
    descriptor.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    descriptor.BlendState = blend_state;
    descriptor.DepthStencilState = depth_state;
    descriptor.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descriptor.SampleMask = UINT_MAX;
    descriptor.RasterizerState = rasterizer_description;
    descriptor.NumRenderTargets = 1;
    descriptor.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    switch (primitive_type)
    {
        case PrimitiveType::TRIANGLES:
            descriptor.PrimitiveTopologyType =
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            break;
        case PrimitiveType::LINES:
            descriptor.PrimitiveTopologyType =
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
            break;
    }

    // create pipeline state
    if (device->CreateGraphicsPipelineState(&descriptor, IID_PPV_ARGS(&pso_)) !=
        S_OK)
    {
        throw Exception("could not create pso");
    }
}

D3D12Material::D3D12Material(
    const std::vector<D3D12_INPUT_ELEMENT_DESC> &input_descriptors,
    Texture *multisample_source)
    : pso_()
    , textures_({multisample_source})
{
    const auto vertex_source = R"(

cbuffer DefaultUniforms : register(b0)
{
    matrix projection;
    matrix view;
    float4 camera;
    matrix model;
    matrix normal_matrix;
    matrix bones[100];
    float4 light;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 frag_position : POSITION0;
};

PSInput main(
    float4 position : TEXCOORD0,
    float4 normal : TEXCOORD1,
    float4 colour : TEXCOORD2,
    float4 tex_coord : TEXCOORD3,
    float4 tangent : TEXCOORD4,
    float4 bitangent : TEXCOORD5,
    uint4 bone_ids : TEXCOORD6,
    float4 bone_weights : TEXCOORD7)
{
    PSInput result;

    result.frag_position = mul(position, model);
    result.position = mul(result.frag_position, view);
    result.position = mul(result.position, projection);

    return result;
}
    )";

    const auto fragment_source = R"(
Texture2DMS<float> g_multisample_depth : register(t0);

struct PSInput
{
    float4 position : SV_POSITION;
};

float4 main(PSInput input, float4 Pos : SV_Position) : SV_TARGET
{
    float r = g_multisample_depth.sample[0][input.position.xy].r;
    return float4(r, 0.0, 0.0, 1);
}
    )";

    const auto vertex_shader = create_shader(vertex_source, ShaderType::VERTEX);
    const auto fragment_shader =
        create_shader(fragment_source, ShaderType::FRAGMENT);

    auto *device = D3D12Context::device();
    auto *root_signature = D3D12Context::root_signature();

    auto blend_state = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    // set blend mode based on light
    // ambient is always rendered first (no blending)
    // directional and point are always rendered after (blending)
    blend_state.RenderTarget[0].BlendEnable = FALSE;

    auto depth_state = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    depth_state.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    D3D12_RASTERIZER_DESC rasterizer_description = {0};
    rasterizer_description.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizer_description.CullMode = D3D12_CULL_MODE_BACK;
    rasterizer_description.FrontCounterClockwise = TRUE;
    rasterizer_description.DepthClipEnable = FALSE;
    rasterizer_description.ConservativeRaster =
        D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC descriptor = {};
    descriptor.SampleDesc.Count = 1u;
    descriptor.SampleDesc.Quality = 0u;

    descriptor.InputLayout = {
        input_descriptors.data(), static_cast<UINT>(input_descriptors.size())};
    descriptor.pRootSignature = root_signature;
    descriptor.VS = CD3DX12_SHADER_BYTECODE(vertex_shader.Get());
    descriptor.PS = CD3DX12_SHADER_BYTECODE(fragment_shader.Get());
    descriptor.RasterizerState = rasterizer_description;
    descriptor.BlendState = blend_state;
    descriptor.DepthStencilState = depth_state;
    descriptor.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descriptor.SampleMask = UINT_MAX;
    descriptor.NumRenderTargets = 1;
    descriptor.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    descriptor.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // create pipeline state
    if (device->CreateGraphicsPipelineState(&descriptor, IID_PPV_ARGS(&pso_)) !=
        S_OK)
    {
        throw Exception("could not create pso");
    }
}

std::vector<Texture *> D3D12Material::textures() const
{
    return textures_;
}

ID3D12PipelineState *D3D12Material::pso() const
{
    return pso_.Get();
}

}
