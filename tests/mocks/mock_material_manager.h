////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#include <gmock/gmock.h>

#include "graphics/lights/light_type.h"
#include "graphics/material.h"
#include "graphics/material_manager.h"
#include "graphics/render_entity.h"


class MockMaterialManager : public iris::MaterialManager
{
  public:
    MOCK_METHOD(
        iris::Material *,
        create,
        (iris::RenderGraph *, iris::RenderEntity *, iris::LightType, bool, bool, bool, bool),
        (override));
    MOCK_METHOD(void, clear, (), (override));
};
