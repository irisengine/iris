#pragma once

#include "graphics/pixel_format.h"
#include "graphics/texture.h"

class FakeTexture : public iris::Texture
{
  public:
    FakeTexture()
        : iris::Texture({}, 1u, 1u, iris::PixelFormat::RGBA)
    {
    }

    ~FakeTexture() override = default;
};
