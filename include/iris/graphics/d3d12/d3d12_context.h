////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#include <dxgi1_6.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

namespace iris
{

/**
 * This class provides singleton access to various D3D12 primitives.
 */
class D3D12Context
{
  public:
    /**
     * Get the DXGI factory.c
     *
     * @returns
     *   DXGI factory.
     */
    static IDXGIFactory4 *dxgi_factory();

    /**
     * Get the D3D12 device.
     *
     * @returns
     *   D3D12 device.
     */
    static ID3D12Device2 *device();

  private:
    // private to force access through above public static methods
    D3D12Context();
    static D3D12Context &instance();

    // these are the member function equivalents of the above static methods
    // see their docs for details

    IDXGIFactory4 *dxgi_factory_impl() const;

    ID3D12Device2 *device_impl() const;

    /** D3D12 handle to dxgi factory. */
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi_factory_;

    /** D3D12 handle to d3d12 device. */
    Microsoft::WRL::ComPtr<ID3D12Device2> device_;

    /** D3D12 handle to d3d12 info queue. */
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> info_queue_;
};

}
