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

    // hide warnings we don't care about
    D3D12_MESSAGE_ID hide[] = {
        D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
        D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_RENDERTARGETVIEW_NOT_SET};

    D3D12_INFO_QUEUE_FILTER filter = {0};
    filter.DenyList.NumIDs = _countof(hide);
    filter.DenyList.pIDList = hide;
    info_queue_->AddStorageFilterEntries(&filter);

    // set break on error and warning
    info_queue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    info_queue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
    info_queue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
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

IDXGIFactory4 *D3D12Context::dxgi_factory_impl() const
{
    return dxgi_factory_.Get();
}

ID3D12Device2 *D3D12Context::device_impl() const
{
    return device_.Get();
}

}
