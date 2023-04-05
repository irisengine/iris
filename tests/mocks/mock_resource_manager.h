////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string_view>

#include <gmock/gmock.h>

#include "core/data_buffer.h"
#include "core/resource_manager.h"

class MockResourceManager : public iris::ResourceManager
{
  public:
    MOCK_METHOD(bool, exists, (std::string_view), (const override));

  protected:
    MOCK_METHOD(iris::DataBuffer, do_load, (std::string_view), (override));
    MOCK_METHOD(void, do_save, (std::string_view, const iris::DataBuffer &), (override));
};
