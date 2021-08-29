#pragma once

#include <vector>

#include "graphics/material.h"
#include "graphics/texture.h"

class FakeMaterial : public iris::Material
{
  public:
    ~FakeMaterial() override = default;

    std::vector<iris::Texture *> textures() const override
    {
        return {};
    }
};
