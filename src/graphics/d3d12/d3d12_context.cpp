////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_context.h"

#include <dxgi1_6.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/error_handling.h"

namespace iris
{

D3D12Context::D3D12Context()
    : dxgi_factory_(nullptr)
    , device_(nullptr)
    , info_queue_(nullptr)
    , root_signature_(nullptr)
    , num_descriptors_(0u)
{
    // create and enable a debug layer
    Microsoft::WRL::ComPtr<ID3D12Debug> debug_interface = nullptr;
    expect(::D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface)) == S_OK, "could not create debug interface");

    debug_interface->EnableDebugLayer();

    expect(
        ::CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgi_factory_)) == S_OK,
        "could not create dxgi factory");

    Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgi_adaptor_tmp = nullptr;
    Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgi_adaptor = nullptr;
    SIZE_T max_dedicated_memory = 0u;
    UINT32 index = 0u;

    // search all adaptors for one that can be used to create a d3d12 device
    // (with the most amount of dedicated video memory)
    while (dxgi_factory_->EnumAdapters1(index++, &dxgi_adaptor_tmp) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 adaptor_descriptor = {0};
        dxgi_adaptor_tmp->GetDesc1(&adaptor_descriptor);

        // ignore the software renderer
        if ((adaptor_descriptor.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0)
        {
            // test if we can create a d3d12 device with this adaptor
            if (::D3D12CreateDevice(dxgi_adaptor_tmp.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr) ==
                S_FALSE)
            {
                // check if this adaptor has more available memory
                if (adaptor_descriptor.DedicatedVideoMemory > max_dedicated_memory)
                {
                    const auto cast = dxgi_adaptor_tmp.As(&dxgi_adaptor);
                    expect(cast == S_OK, "failed to cast dxgi adaptor");

                    max_dedicated_memory = adaptor_descriptor.DedicatedVideoMemory;
                }
            }
        }
    }

    ensure(dxgi_adaptor != nullptr, "could not find a directx12 adapter");

    // create actual d3d12 device
    expect(
        ::D3D12CreateDevice(dxgi_adaptor.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device_)) == S_OK,
        "could not create directx12 device");

    expect(device_.As(&info_queue_) == S_OK, "could not cast device to info queue");

    // set break on error and warning
    info_queue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    info_queue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
    info_queue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

    CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
    CD3DX12_ROOT_PARAMETER1 root_parameters[2];

    // see D3D12Renderer source for usage
    static const auto num_cbv_descriptors = 2u;
    static const auto num_srv_descriptors = 5u;
    num_descriptors_ = num_cbv_descriptors + num_srv_descriptors;

    // setup root signature

    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, num_cbv_descriptors, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, num_srv_descriptors, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

    root_parameters[0].InitAsDescriptorTable(2, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);
    root_parameters[1].InitAsDescriptorTable(2, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

    D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                                                      D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                                                      D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                                                      D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

    // create a sampler to store in the root signature
    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
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
    root_signature_description.Init_1_1(_countof(root_parameters), root_parameters, 1u, &sampler, root_signature_flags);

    Microsoft::WRL::ComPtr<ID3DBlob> signature = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> error = nullptr;
    if (::D3DX12SerializeVersionedRootSignature(
            &root_signature_description, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error) != S_OK)
    {
        const std::string error_message(static_cast<char *>(error->GetBufferPointer()), error->GetBufferSize());

        throw iris::Exception("root signature serialization failed: " + error_message);
    }

    expect(
        device_->CreateRootSignature(
            0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&root_signature_)) == S_OK,
        "could not create root signature");
}

D3D12Context &D3D12Context::instance()
{
    static D3D12Context instance{};
    return instance;
}

IDXGIFactory4 *D3D12Context::dxgi_factory()
{
    return instance().dxgi_factory_impl();
}

ID3D12Device2 *D3D12Context::device()
{
    return instance().device_impl();
}

ID3D12RootSignature *D3D12Context::root_signature()
{
    return instance().root_signature_impl();
}

std::uint32_t D3D12Context::num_descriptors()
{
    return instance().num_descriptors_impl();
}

IDXGIFactory4 *D3D12Context::dxgi_factory_impl() const
{
    return dxgi_factory_.Get();
}

ID3D12Device2 *D3D12Context::device_impl() const
{
    return device_.Get();
}

ID3D12RootSignature *D3D12Context::root_signature_impl() const
{
    return root_signature_.Get();
}

std::uint32_t D3D12Context::num_descriptors_impl() const
{
    return num_descriptors_;
}

}
