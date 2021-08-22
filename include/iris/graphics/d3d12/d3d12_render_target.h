#pragma once

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
        std::unique_ptr<D3D12Texture> depth_texture);

    ~D3D12RenderTarget() override = default;

    /**
     * Get descriptor handle to render target.
     *
     * @returns
     *   D3D12 descriptor handle.
     */
    D3D12DescriptorHandle handle() const;

  private:
    /** D3D12 descriptor handle. */
    D3D12DescriptorHandle handle_;
};

}
