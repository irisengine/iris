////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <memory>

#include <gmock/gmock.h>

#include "core/quaternion.h"
#include "core/vector3.h"
#include "mocks/mock_script.h"
#include "scripting/script_runner.h"

TEST(script_runner, no_args_no_results)
{
    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, execute(0u, 0u));

    iris::ScriptRunner runner(std::move(script));
    runner.execute("func");
}

TEST(script_runner, single_bool_argument)
{
    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<bool>(false)));
    EXPECT_CALL(*script, execute(1u, 0u));

    iris::ScriptRunner runner(std::move(script));
    runner.execute("func", false);
}

TEST(script_runner, single_int_argument)
{
    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<std::int32_t>(123)));
    EXPECT_CALL(*script, execute(1u, 0u));

    iris::ScriptRunner runner(std::move(script));
    runner.execute("func", 123);
}

TEST(script_runner, single_float_argument)
{
    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<float>(3.34f)));
    EXPECT_CALL(*script, execute(1u, 0u));

    iris::ScriptRunner runner(std::move(script));
    runner.execute("func", 3.34f);
}

TEST(script_runner, single_c_string_argument)
{
    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<const char *>(::testing::StrEq("c_string"))));
    EXPECT_CALL(*script, execute(1u, 0u));

    iris::ScriptRunner runner(std::move(script));
    runner.execute("func", "c_string");
}

TEST(script_runner, single_string_argument)
{
    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<const std::string &>("string")));
    EXPECT_CALL(*script, execute(1u, 0u));

    iris::ScriptRunner runner(std::move(script));
    runner.execute("func", std::string("string"));
}

TEST(script_runner, single_vector3_argument)
{
    iris::Vector3 vec{1.1f, 2.2f, 3.3f};

    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<const iris::Vector3 &>(vec)));
    EXPECT_CALL(*script, execute(1u, 0u));

    iris::ScriptRunner runner(std::move(script));
    runner.execute("func", vec);
}

TEST(script_runner, single_quaternion_argument)
{
    iris::Quaternion q{1.0f, 2.0f, 3.0f, 4.0f};

    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<const iris::Quaternion &>(q)));
    EXPECT_CALL(*script, execute(1u, 0u));

    iris::ScriptRunner runner(std::move(script));
    runner.execute("func", q);
}

TEST(script_runner, single_bool_result)
{
    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, execute(0u, 1u));
    EXPECT_CALL(*script, get_result(::testing::Matcher<bool &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(true));

    iris::ScriptRunner runner(std::move(script));
    EXPECT_EQ(runner.execute<bool>("func"), true);
}

TEST(script_runner, single_int_result)
{
    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, execute(0u, 1u));
    EXPECT_CALL(*script, get_result(::testing::Matcher<std::int32_t &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(321));

    iris::ScriptRunner runner(std::move(script));
    EXPECT_EQ(runner.execute<std::int32_t>("func"), 321);
}

TEST(script_runner, single_float_result)
{
    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, execute(0u, 1u));
    EXPECT_CALL(*script, get_result(::testing::Matcher<float &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(4.567f));

    iris::ScriptRunner runner(std::move(script));
    EXPECT_EQ(runner.execute<float>("func"), 4.567f);
}

TEST(script_runner, single_string_result)
{
    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, execute(0u, 1u));
    EXPECT_CALL(*script, get_result(::testing::Matcher<std::string &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>("abcd"));

    iris::ScriptRunner runner(std::move(script));
    EXPECT_EQ(runner.execute<std::string>("func"), "abcd");
}

TEST(script_runner, single_vector3_result)
{
    iris::Vector3 vec{1.1f, 2.2f, 3.3f};

    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, execute(0u, 1u));
    EXPECT_CALL(*script, get_result(::testing::Matcher<iris::Vector3 &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(vec));

    iris::ScriptRunner runner(std::move(script));
    EXPECT_EQ(runner.execute<iris::Vector3>("func"), vec);
}

TEST(script_runner, single_quaternion_result)
{
    iris::Quaternion q{1.0f, 2.0f, 3.0f, 4.0f};

    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, execute(0u, 1u));
    EXPECT_CALL(*script, get_result(::testing::Matcher<iris::Quaternion &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(q));

    iris::ScriptRunner runner(std::move(script));
    EXPECT_EQ(runner.execute<iris::Quaternion>("func"), q);
}

TEST(script_runner, multiple_arguments)
{
    iris::Vector3 vec{1.1f, 2.2f, 3.3f};
    iris::Quaternion q{1.0f, 2.0f, 3.0f, 4.0f};

    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<bool>(true)));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<int>(-10)));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<float>(7.8f)));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<const char *>(::testing::StrEq("str1"))));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<const std::string &>("str2")));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<const iris::Vector3 &>(vec)));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<const iris::Quaternion &>(q)));
    EXPECT_CALL(*script, execute(7u, 0u));

    iris::ScriptRunner runner(std::move(script));
    runner.execute("func", true, -10, 7.8f, "str1", std::string("str2"), vec, q);
}

TEST(script_runner, multiple_results)
{
    iris::Vector3 vec{1.1f, 2.2f, 3.3f};
    iris::Quaternion q{1.0f, 2.0f, 3.0f, 4.0f};

    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, execute(0u, 6u));
    EXPECT_CALL(*script, get_result(::testing::Matcher<iris::Quaternion &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(q));
    EXPECT_CALL(*script, get_result(::testing::Matcher<iris::Vector3 &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(vec));
    EXPECT_CALL(*script, get_result(::testing::Matcher<std::string &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>("efg"));
    EXPECT_CALL(*script, get_result(::testing::Matcher<float &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(99.9f));
    EXPECT_CALL(*script, get_result(::testing::Matcher<std::int32_t &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(-32));
    EXPECT_CALL(*script, get_result(::testing::Matcher<bool &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(true));

    iris::ScriptRunner runner(std::move(script));
    EXPECT_EQ(
        (runner.execute<bool, std::int32_t, float, std::string, iris::Vector3, iris::Quaternion>("func")),
        (std::make_tuple(true, -32, 99.9f, std::string("efg"), vec, q)));
}

TEST(script_runner, multiple_arguments_and_multiple_results)
{
    iris::Vector3 vec{1.1f, 2.2f, 3.3f};
    iris::Quaternion q{1.0f, 2.0f, 3.0f, 4.0f};

    ::testing::InSequence s;

    auto script = std::make_unique<MockScript>();
    EXPECT_CALL(*script, set_function("func"));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<bool>(true)));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<int>(-10)));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<float>(7.8f)));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<const char *>(::testing::StrEq("str1"))));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<const std::string &>("str2")));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<const iris::Vector3 &>(vec)));
    EXPECT_CALL(*script, set_argument(::testing::Matcher<const iris::Quaternion &>(q)));
    EXPECT_CALL(*script, execute(7u, 6u));

    EXPECT_CALL(*script, get_result(::testing::Matcher<iris::Quaternion &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(q));
    EXPECT_CALL(*script, get_result(::testing::Matcher<iris::Vector3 &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(vec));
    EXPECT_CALL(*script, get_result(::testing::Matcher<std::string &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>("efg"));
    EXPECT_CALL(*script, get_result(::testing::Matcher<float &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(99.9f));
    EXPECT_CALL(*script, get_result(::testing::Matcher<std::int32_t &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(-32));
    EXPECT_CALL(*script, get_result(::testing::Matcher<bool &>(::testing::_)))
        .WillOnce(::testing::SetArgReferee<0>(true));

    iris::ScriptRunner runner(std::move(script));
    EXPECT_EQ(
        (runner.execute<bool, std::int32_t, float, std::string, iris::Vector3, iris::Quaternion>(
            "func", true, -10, 7.8f, "str1", std::string("str2"), vec, q)),
        (std::make_tuple(true, -32, 99.9f, std::string("efg"), vec, q)));
}
