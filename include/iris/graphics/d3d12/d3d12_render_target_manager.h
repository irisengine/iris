////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "graphics/d3d12/d3d12_render_target.h"
#include "graphics/render_target_manager.h"

namespace iris
{

/**
 * Implementation of RenderTargetManager for D3D12.
 */
class D3D12RenderTargetManager : public RenderTargetManager
{
  public:
    ~D3D12RenderTargetManager() override = default;

    /**
     * Create a render target the size of the current window.
     *
     * @returns
     *   Pointer to the created render target.
     */
    RenderTarget *create() override;

    /**
     * Create a render target with the specified dimensions.
     *
     * @param width
     *   Width of render target.
     *
     * @param height
     *   Height of render target.
     *
     * @returns
     *   Pointer to the created render target.
     */
    RenderTarget *create(std::uint32_t width, std::uint32_t height) override;

    /**
     * Create a new render target which combines the colour target and depth target of two separate render targets.
     *
     * @param colour_target
     *   Render target to for colour target.
     *
     * @param depth_target
     *   Render target to for depth target.
     *
     * @returns
     *   Pointer to the created render target.
     */
    RenderTarget *create(const RenderTarget *colour_target, const RenderTarget *depth_target) override;

  private:
    /** Collection of created render targets. */
    std::vector<std::unique_ptr<D3D12RenderTarget>> render_targets_;
};

}
