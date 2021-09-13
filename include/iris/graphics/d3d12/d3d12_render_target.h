#pragma once

#include <cstdint>
#include <memory>

#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/d3d12/d3d12_texture.h"
#include "graphics/render_target.h"

namespace iris
{

/**
 * Implementation of RenderTarget for d3d12.
 */
class D3D12RenderTarget : public RenderTarget
{
  public:
    /**
     * Construct a new D3D12RenderTarget.
     *
     * @param colour_texture
     *   Texture to render colour data to.
     *
     * @param depth_texture
     *   Texture to render depth data to.
     */
    D3D12RenderTarget(
        std::unique_ptr<D3D12Texture> colour_texture,
        std::unique_ptr<D3D12Texture> depth_texture,
        std::uint32_t samples);

    ~D3D12RenderTarget() override = default;

    /**
     * Get descriptor handle to render target.
     *
     * @returns
     *   D3D12 descriptor handle.
     */
    D3D12DescriptorHandle handle() const;

    D3D12DescriptorHandle multisample_handle() const;

    D3D12Texture *multisample_colour_texture() const;

    D3D12Texture *multisample_depth_texture() const;

  private:
    /** D3D12 descriptor handle. */
    D3D12DescriptorHandle handle_;

    D3D12DescriptorHandle multisample_handle_;

    std::unique_ptr<D3D12Texture> multisample_colour_texture_;
    std::unique_ptr<D3D12Texture> multisample_depth_texture_;
};

}
