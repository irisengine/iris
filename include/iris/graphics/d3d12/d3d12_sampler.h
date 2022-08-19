////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/sampler.h"

namespace iris
{

/**
 * Implementation of Sampler for D3D12.
 */
class D3D12Sampler : public Sampler
{
  public:
    /**
     * Create a new D3D12Sampler.
     *
     * @param descriptor
     *   Description of sampler parameters.
     *
     * @param index
     *   Index into the global array of all allocated samplers.
     */
    D3D12Sampler(const SamplerDescriptor &descriptor, std::uint32_t index);
    ~D3D12Sampler();

    /**
     * Get the descriptor handle for the sampler object.
     *
     * @returns
     *   Descriptor handle.
     */
    D3D12DescriptorHandle handle() const;

  private:
    /** Descriptor handle for sampler object. */
    D3D12DescriptorHandle descriptor_;
};

}
