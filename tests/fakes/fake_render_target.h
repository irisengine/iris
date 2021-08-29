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
        : iris::RenderTarget(
              std::make_unique<FakeTexture>(),
              std::make_unique<FakeTexture>())
    {
    }

    ~FakeRenderTarget() override = default;
};
