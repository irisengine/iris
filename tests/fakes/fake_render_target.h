////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>

#include "graphics/render_target.h"
#include "graphics/texture.h"

#include "fakes/fake_texture.h"

class FakeRenderTarget : public iris::RenderTarget
{
  public:
    FakeRenderTarget()
        : colour_(std::make_unique<FakeTexture>())
        , depth_(std::make_unique<FakeTexture>())
        , iris::RenderTarget(colour_.get(), depth_.get())
    {
    }

    ~FakeRenderTarget() override = default;

    std::unique_ptr<FakeTexture> colour_;
    std::unique_ptr<FakeTexture> depth_;
};
