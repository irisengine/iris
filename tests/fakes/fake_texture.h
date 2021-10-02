#pragma once

#include "graphics/texture.h"
#include "graphics/texture_usage.h"

class FakeTexture : public iris::Texture
{
  public:
    FakeTexture()
        : iris::Texture({}, 1u, 1u, iris::TextureUsage::IMAGE)
    {
    }

    ~FakeTexture() override = default;
};
