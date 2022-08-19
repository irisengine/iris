////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>

#include "fakes/fake_texture.h"
#include "graphics/render_target.h"
#include "graphics/texture.h"


#include "fakes/fake_texture.h"

class FakeRenderTarget : public iris::RenderTarget
{
  public:
    FakeRenderTarget()
        : iris::RenderTarget(new FakeTexture{}, new FakeTexture{})
    {
    }

    ~FakeRenderTarget() override = default;
};
