////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#include <gmock/gmock.h>

#include "graphics/render_target.h"
#include "graphics/render_target_manager.h"

class MockRenderTargetManager : public iris::RenderTargetManager
{
  protected:
    MOCK_METHOD(iris::RenderTarget *, create, (), (override));
    MOCK_METHOD(iris::RenderTarget *, create, (std::uint32_t, std::uint32_t), (override));
    MOCK_METHOD(iris::RenderTarget *, create, (const iris::RenderTarget *, const iris::RenderTarget *), (override));
};
