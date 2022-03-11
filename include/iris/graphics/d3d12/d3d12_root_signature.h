////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>
#include <tuple>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/error_handling.h"
#include "graphics/d3d12/d3d12_constant_buffer.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/d3d12/d3d12_structured_buffer.h"
#include "log/log.h"

namespace iris
{

// defined below are the various parameter types that can be passed to D3D12RootSignature (as template arguments)
// each one must have:
//  - size : how many DWORDS it takes up in the root signature
//  - init_parameter : method for initialising a d3d12 root parameter object.
//  - encode_set : method for encoding a value into a command list

template <std::uint32_t ShaderRegister, std::uint32_t RegisterSpace, D3D12_SHADER_VISIBILITY ShaderVisibility>
struct ConstantParameter
{
    static constexpr auto size = 1u;

    static void init_parameter(CD3DX12_ROOT_PARAMETER1 &parameter)
    {
        parameter.InitAsConstants(1u, ShaderRegister, RegisterSpace, ShaderVisibility);
    }

    static void encode_set(ID3D12GraphicsCommandList *command_list, std::uint32_t index, std::uint32_t value)
    {
        command_list->SetGraphicsRoot32BitConstant(index, value, 0u);
    }
};

template <std::uint32_t ShaderRegister, std::uint32_t RegisterSpace, D3D12_SHADER_VISIBILITY ShaderVisibility>
struct ConstantBufferViewParameter
{
    static constexpr auto size = 2u;

    static void init_parameter(CD3DX12_ROOT_PARAMETER1 &parameter)
    {
        parameter.InitAsConstantBufferView(ShaderRegister, RegisterSpace);
    }

    static void encode_set(
        ID3D12GraphicsCommandList *command_list,
        std::uint32_t index,
        const D3D12ConstantBuffer *buffer)
    {
        command_list->SetGraphicsRootConstantBufferView(index, buffer->resource()->GetGPUVirtualAddress());
    }
};

template <std::uint32_t ShaderRegister, std::uint32_t RegisterSpace, D3D12_SHADER_VISIBILITY ShaderVisibility>
struct ShaderResourceViewParameter
{
    static constexpr auto size = 2u;

    static void init_parameter(CD3DX12_ROOT_PARAMETER1 &parameter)
    {
        parameter.InitAsShaderResourceView(ShaderRegister, RegisterSpace);
    }

    static void encode_set(
        ID3D12GraphicsCommandList *command_list,
        std::uint32_t index,
        const D3D12ConstantBuffer *buffer)
    {
        command_list->SetGraphicsRootConstantBufferView(index, buffer->resource()->GetGPUVirtualAddress());
    }

    static void encode_set(
        ID3D12GraphicsCommandList *command_list,
        std::uint32_t index,
        const D3D12StructuredBuffer *buffer)
    {
        command_list->SetGraphicsRootShaderResourceView(index, buffer->resource()->GetGPUVirtualAddress());
    }
};

template <
    D3D12_DESCRIPTOR_RANGE_TYPE RangeType,
    std::uint32_t NumDescriptors,
    std::uint32_t ShaderRegister,
    std::uint32_t RegisterSpace,
    D3D12_SHADER_VISIBILITY ShaderVisibility>
struct TableParameter
{
    static constexpr auto size = 1u;

    static void init_parameter(CD3DX12_ROOT_PARAMETER1 &parameter)
    {
        static CD3DX12_DESCRIPTOR_RANGE1 range{};
        range.Init(RangeType, NumDescriptors, ShaderRegister, RegisterSpace);

        parameter.InitAsDescriptorTable(1u, &range, ShaderVisibility);
    }

    static void encode_set(
        ID3D12GraphicsCommandList *command_list,
        std::uint32_t index,
        const D3D12DescriptorHandle &handle)
    {
        command_list->SetGraphicsRootDescriptorTable(index, handle.gpu_handle());
    }
};

/**
 * A class encapsulating a d3d12 root signature - allows for root parameters to be described (at compile time) and
 * provides a runtime method for encoding arguments.
 */
template <class... Parameters>
class D3D12RootSignature
{
  public:
    /**
     * Construct a new D3D12RootSignature.
     */
    D3D12RootSignature()
        : root_signature_(nullptr)
    {
        static_assert((Parameters::size + ...) <= 64, "exceeded maximum root signature size");

        // create a root parameter object for all the provided template parameters.
        std::array<CD3DX12_ROOT_PARAMETER1, sizeof...(Parameters)> root_parameters;
        auto i = 0u;

        // call the init_parameter method for all the parameters
        (Parameters::init_parameter(root_parameters[i++]), ...);

        D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                                                          D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                                                          D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                                                          D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        // create a sampler to store in the root signature
        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_description{};
        root_signature_description.Init_1_1(
            static_cast<UINT>(root_parameters.size()), root_parameters.data(), 1u, &sampler, root_signature_flags);

        Microsoft::WRL::ComPtr<ID3DBlob> signature = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> error = nullptr;
        if (::D3DX12SerializeVersionedRootSignature(
                &root_signature_description, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error) != S_OK)
        {
            const std::string error_message(static_cast<char *>(error->GetBufferPointer()), error->GetBufferSize());

            LOG_ENGINE_ERROR("d3d12_context", "{}", error_message);

            throw iris::Exception("root signature serialization failed: " + error_message);
        }

        auto *device = D3D12Context::device();

        expect(
            device->CreateRootSignature(
                0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&root_signature_)) == S_OK,
            "could not create root signature");
    }

    /**
     * Encode a pack of commands. Each one should correspond to Parameter.
     *
     * @param args
     *   Pack of arguments, one for each Parameter.
     */
    template <class... Args>
    void encode_arguments(ID3D12GraphicsCommandList *command_list, Args &&...args)
    {
        auto i = 0u;

        // call encode_set of each parameter where each one gets its associated argument
        (Parameters::encode_set(command_list, i++, args), ...);
    }

    /**
     * Get a handle to the D3D12 root signature.
     *
     * @returns
     *   D3D12 root signature object.
     */
    ID3D12RootSignature *handle() const
    {
        return root_signature_.Get();
    }

  private:
    /** D3D12 root signature object. */
    Microsoft::WRL::ComPtr<ID3D12RootSignature> root_signature_;
};

}
