////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>
#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/data_buffer.h"
#include "graphics/cube_map.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"

namespace iris
{

/**
 * Implementation of CubeMap for d3d12.
 */
class D3D12CubeMap : public CubeMap
{
  public:
    /**
     * Construct a new D3D12CubeMap.
     *
     * @param right_data
     *   Image data for right face of cube.
     *
     * @param left_data
     *   Image data for left face of cube.
     *
     * @param top_data
     *   Image data for top face of cube.
     *
     * @param bottom_data
     *   Image data for bottom face of cube.
     *
     * @param back_data
     *   Image data for back face of cube.
     *
     * @param front_data
     *   Image data for front face of cube.
     *
     * @param width
     *   Width of each image face.
     *
     * @param height
     *   Height of each image face.
     *
     * @param sampler
     *   Sampler to use for this cube map.
     *
     * @param index
     *   Index into the global array of all allocated textures.
     */
    D3D12CubeMap(
        const DataBuffer &right_data,
        const DataBuffer &left_data,
        const DataBuffer &top_data,
        const DataBuffer &bottom_data,
        const DataBuffer &back_data,
        const DataBuffer &front_data,
        std::uint32_t width,
        std::uint32_t height,
        const Sampler *sampler,
        std::uint32_t index);

    ~D3D12CubeMap() override = default;

    /*
     * Get the handle to the d3d12 resource where the image data will be copied
     * to.
     *
     * @returns
     *   D3D12 resource.
     */
    ID3D12Resource *resource() const;

    /**
     * Get the handle to the d3d12 resource where the image data is initially
     * uploaded to
     *
     * @returns
     *   D3D12 resource.
     */
    ID3D12Resource *upload() const;

    /**
     * Get the handle to the image resource view.
     *
     * @returns
     *   Resource view handle.
     */
    D3D12DescriptorHandle handle() const;

    /**
     * Get the d3d12 footprints describing the image data layout for each cube face.
     *
     * @returns
     *   D3D12 footprints.
     */
    std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT, 6u> footprints() const;

  private:
    /** Handle to resource where image data will be coped to. */
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

    /** Handle to resource where image data is uploaded to. */
    Microsoft::WRL::ComPtr<ID3D12Resource> upload_;

    /** Resource view to image data. */
    D3D12DescriptorHandle resource_view_;

    /** Type of heap to copy data to. */
    D3D12_DESCRIPTOR_HEAP_TYPE type_;

    /** Footprint describing the image data layout for each cube face. */
    std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT, 6u> footprints_;
};

}
