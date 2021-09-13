#pragma once

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/data_buffer.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/pixel_format.h"
#include "graphics/texture.h"

namespace iris
{

/**
 * Implementation of Texture for d3d12.
 *
 * Internally texture data is first copied to an upload heap. The renderer can
 * then encode a command to copy that data to a shader visible heap.
 */
class D3D12Texture : public Texture
{
  public:
    /**
     * Construct a new D3D12Texture.
     *
     * @param data
     *   Raw data of image, in pixel_format.
     *
     * @param width
     *   Width of image.
     *
     * @param height
     *   Height of image.
     *
     * @param pixel_format
     *   Number of channels.
     *
     * @param is_render_target
     *   Flag indicating if this texture will be used as a render target.
     */
    D3D12Texture(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        PixelFormat pixel_format,
        bool is_render_target = false,
        std::uint32_t samples = 1u);

    /**
     * Construct a new D3D12Texture for a depth buffer. Whilst it might not be
     * obvious from the api that this constructor is for creating depth buffers,
     * this is an internal class and the engine knows when to use each
     * constructor.
     *
     * @param width
     *   Width of image.
     *
     * @param height
     *   Height of image.
     */
    D3D12Texture(
        std::uint32_t width,
        std::uint32_t height,
        std::uint32_t samples = 1u);

    ~D3D12Texture() override = default;

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
     * Get the d3d12 footprint describing the image data layout.
     *
     * @returns
     *   D3D12 footprint.
     */
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint() const;

    /**
     * Get the handle to the image resource view. Only valid if the object was
     * constructed for non-depth buffer usage.
     *
     * @returns
     *   Resource view handle.
     */
    D3D12DescriptorHandle handle() const;

    /**
     * Get the handle to the image resource view. Only valid if the object was
     * constructed for depth buffer usage.
     *
     * @returns
     *   Resource view handle (depth buffer only).
     */
    D3D12DescriptorHandle depth_handle() const;

    /**
     * Get the type of heap image data will be copied to.
     *
     * @returns
     *   Heap type.
     */
    D3D12_DESCRIPTOR_HEAP_TYPE type() const;

  private:
    /** Handle to resource where image data will be coped to. */
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

    /** Handle to resource where image data is uploaded to. */
    Microsoft::WRL::ComPtr<ID3D12Resource> upload_;

    /** Resource view to image data. */
    D3D12DescriptorHandle resource_view_;

    /** Resource view to image data (depth only). */
    D3D12DescriptorHandle depth_resource_view_;

    /** Footprint describing image data layout. */
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint_;

    /** Type of heap to copy data to. */
    D3D12_DESCRIPTOR_HEAP_TYPE type_;
};

}
