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
  protected:
    MOCK_METHOD(iris::DataBuffer, do_load, (std::string_view), (override));
};
