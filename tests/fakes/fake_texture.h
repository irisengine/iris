////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graphics/texture.h"
#include "graphics/texture_usage.h"

class FakeTexture : public iris::Texture
{
  public:
    FakeTexture()
        : iris::Texture({}, 1u, 1u, nullptr, iris::TextureUsage::IMAGE, 0u)
    {
    }

    ~FakeTexture() override = default;
};
