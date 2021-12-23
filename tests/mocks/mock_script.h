////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#include <gmock/gmock.h>

#include "core/quaternion.h"
#include "core/vector3.h"
#include "scripting/script.h"

class MockScript : public iris::Script
{
  public:
    ~MockScript() override = default;

    MOCK_METHOD(void, set_function, (const std::string &), (override));
    MOCK_METHOD(void, set_argument, (bool), (override));
    MOCK_METHOD(void, set_argument, (std::int32_t), (override));
    MOCK_METHOD(void, set_argument, (float), (override));
    MOCK_METHOD(void, set_argument, (const char *), (override));
    MOCK_METHOD(void, set_argument, (const std::string &), (override));
    MOCK_METHOD(void, set_argument, (const iris::Vector3 &), (override));
    MOCK_METHOD(void, set_argument, (const iris::Quaternion &), (override));

    MOCK_METHOD(void, execute, (std::uint32_t, std::uint32_t), (override));

    MOCK_METHOD(void, get_result, (bool &), (override));
    MOCK_METHOD(void, get_result, (std::int32_t &), (override));
    MOCK_METHOD(void, get_result, (float &), (override));
    MOCK_METHOD(void, get_result, (std::string &), (override));
    MOCK_METHOD(void, get_result, (iris::Vector3 &), (override));
    MOCK_METHOD(void, get_result, (iris::Quaternion &), (override));
};
