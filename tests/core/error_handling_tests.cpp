////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/auto_release.h"
#include "core/error_handling.h"

namespace
{

using AutoIntPtr = iris::AutoRelease<int *, nullptr>;

void deleter(int *value)
{
    --*value;
}

}

TEST(expect, bool_expectation_true)
{
    ASSERT_NO_FATAL_FAILURE(iris::expect(true, ""));
}

TEST(expectDeathTest, bool_expectation_false)
{
    ASSERT_DEATH(
        iris::expect(false, "msg"), "msg -> .*error_handling_tests\\.cpp.*");
}

TEST(expect, function_expectation_true)
{
    std::function<std::optional<std::string>(std::string_view)> func =
        [](auto) { return std::optional<std::string>{}; };

    ASSERT_NO_FATAL_FAILURE(iris::expect(func, ""));
}

TEST(expectDeathTest, function_expectation_false)
{
    std::function<std::optional<std::string>(std::string_view)> func =
        [](auto) { return std::optional<std::string>{"err: 1"}; };

    ASSERT_DEATH(
        iris::expect(func, "msg"), "err: 1 -> .*error_handling_tests\\.cpp.*");
}

TEST(expect, unique_ptr_expectation_true)
{
    auto ptr = std::make_unique<int>(0);

    ASSERT_NO_FATAL_FAILURE(iris::expect(ptr, ""));
}

TEST(expectDeathTest, unique_ptr_expectation_false)
{
    std::unique_ptr<int> ptr;

    ASSERT_DEATH(
        iris::expect(ptr, "msg"), "msg -> .*error_handling_tests\\.cpp.*");
}

TEST(expect, auto_release_expectation_true)
{
    int x = 1;
    AutoIntPtr auto_release{&x, deleter};

    ASSERT_NO_FATAL_FAILURE(iris::expect(auto_release, ""));
}

TEST(expectDeathTest, auto_release_expectation_false)
{
    AutoIntPtr auto_release{};

    ASSERT_DEATH(
        iris::expect(auto_release, "msg"),
        "msg -> .*error_handling_tests\\.cpp.*");
}

TEST(ensure, bool_expectation_true)
{
    ASSERT_NO_THROW(iris::ensure(true, ""));
}

TEST(ensure, bool_expectation_false)
{
    auto thrown = false;
    try
    {
        iris::ensure(false, "msg");
    }
    catch (iris::Exception &e)
    {
        ASSERT_THAT(
            e.what(),
            ::testing::MatchesRegex("msg -> .*error_handling_tests\\.cpp.*"));
        thrown = true;
    }

    ASSERT_TRUE(thrown);
}

TEST(ensure, function_expectation_true)
{
    std::function<std::optional<std::string>(std::string_view)> func =
        [](auto) { return std::optional<std::string>{}; };

    ASSERT_NO_THROW(iris::ensure(func, ""));
}

TEST(ensure, function_expectation_false)
{
    auto thrown = false;

    try
    {
        std::function<std::optional<std::string>(std::string_view)> func =
            [](auto) { return std::optional<std::string>{"err: 1"}; };

        iris::ensure(func, "msg");
    }
    catch (iris::Exception &e)
    {
        ASSERT_THAT(
            e.what(),
            ::testing::MatchesRegex(
                "err: 1 -> .*error_handling_tests\\.cpp.*"));
        thrown = true;
    }

    ASSERT_TRUE(thrown);
}

TEST(ensure, unique_ptr_expectation_true)
{
    auto ptr = std::make_unique<int>(0);

    ASSERT_NO_THROW(iris::ensure(ptr, ""));
}

TEST(ensure, unique_ptr_expectation_false)
{
    auto thrown = false;

    try
    {
        std::unique_ptr<int> ptr;

        iris::ensure(ptr, "msg");
    }
    catch (iris::Exception &e)
    {
        ASSERT_THAT(
            e.what(),
            ::testing::MatchesRegex("msg -> .*error_handling_tests\\.cpp.*"));
        thrown = true;
    }

    ASSERT_TRUE(thrown);
}

TEST(ensure, auto_release_expectation_true)
{
    int x = 1;
    AutoIntPtr auto_release{&x, deleter};

    ASSERT_NO_THROW(iris::ensure(auto_release, ""));
}

TEST(ensure, auto_release_expectation_false)
{
    auto thrown = false;

    try
    {
        AutoIntPtr auto_release{};

        iris::ensure(auto_release, "msg");
    }
    catch (iris::Exception &e)
    {
        ASSERT_THAT(
            e.what(),
            ::testing::MatchesRegex("msg -> .*error_handling_tests\\.cpp.*"));
        thrown = true;
    }

    ASSERT_TRUE(thrown);
}
