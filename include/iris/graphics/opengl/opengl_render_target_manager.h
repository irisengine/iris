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


namespace iris
{

class OpenGLRenderTargetManager : public RenderTargetManager
{
  public:
    ~OpenGLRenderTargetManager() override = default;

    RenderTarget *create() override;
    RenderTarget *create(std::uint32_t width, std::uint32_t height) override;
    RenderTarget *create(const RenderTarget *colour_target, const RenderTarget *depth_target) override;

  private:
    std::vector<std::unique_ptr<OpenGLRenderTarget>> render_targets_;
};

}
