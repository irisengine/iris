////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "graphics/opengl/opengl_render_target.h"
#include "graphics/render_target_manager.h"
#include "graphics/texture_manager.h"
#include "graphics/window_manager.h"

namespace iris
{

/**
 * Implementation of RenderTargetManager for OpenGL.
 */
class OpenGLRenderTargetManager : public RenderTargetManager
{
  public:
    /**
     * Create a new OpenGLRenderTargetManager object.
     *
     * @param window_manager
     *   Window manager object.
     *
     * @param texture_manager
     *   Texture manager object.
     */
    OpenGLRenderTargetManager(WindowManager &window_manager, TextureManager &texture_manager);

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
    std::vector<std::unique_ptr<OpenGLRenderTarget>> render_targets_;

    /** Window manager object.  */
    WindowManager &window_manager_;

    /** Texture manager object.  */
    TextureManager &texture_manager_;
};

}
