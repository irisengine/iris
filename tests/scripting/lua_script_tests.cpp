////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <memory>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "core/exception.h"
#include "scripting/lua/lua_script.h"
#include "scripting/script_runner.h"

TEST(lua_script, void_function)
{
    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(function go() print('hello') end)")};
    runner.execute("go");
}

TEST(lua_script, missing_function)
{
    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(function go() print('hello') end)")};
    EXPECT_THROW(runner.execute("go2"), iris::Exception);
}

TEST(lua_script, add_function)
{
    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(function add(a, b) return a + b end)")};
    EXPECT_EQ(runner.execute<int>("add", 3, 4), 7);
}

TEST(lua_script, string_reverse_function)
{
    iris::ScriptRunner runner{
        std::make_unique<iris::LuaScript>(R"(function reverse(str) return string.reverse(str) end)")};
    EXPECT_EQ(runner.execute<std::string>("reverse", "abcd"), "dcba");
}

TEST(lua_script, add_multiple_return)
{
    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function add(x)
                return x + 1, x + 2
            end
        )")};
    EXPECT_EQ((runner.execute<std::int32_t, std::int32_t>("add", 5)), std::make_tuple(6, 7));
}

TEST(lua_script, add_multiple_functions)
{
    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function func1(x)
                return x + 1
            end

            function func2(x)
                return x + 2
            end
        )")};
    EXPECT_EQ((runner.execute<std::int32_t>("func1", 5)), 6);
    EXPECT_EQ((runner.execute<std::int32_t>("func2", 5)), 7);
}

TEST(lua_script, vector3_function)
{
    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function vec3()
                local v = Vector3(1.0, 2.0, 3.0)
                return v:x(), v:y(), v:z()
            end)")};
    EXPECT_EQ((runner.execute<float, float, float>("vec3")), std::make_tuple(1.0f, 2.0f, 3.0f));
}

TEST(lua_script, pass_vector3)
{
    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function split(vec3)
                return vec3:x(), vec3:y(), vec3:z()
            end)")};
    EXPECT_EQ(
        (runner.execute<float, float, float>("split", iris::Vector3(1.1f, 2.2f, 3.3f))),
        std::make_tuple(1.1f, 2.2f, 3.3f));
}

TEST(lua_script, get_vector3)
{
    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function create_vector(x, y, z)
                local vec = Vector3(x, y, z)
                return vec
            end)")};
    EXPECT_EQ((runner.execute<iris::Vector3>("create_vector", 2.1f, 2.2f, 2.3f)), iris::Vector3(2.1f, 2.2f, 2.3f));
}

TEST(lua_script, vector3_setters)
{
    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function set(vec3)
                vec3:set_x(-1.0);
                vec3:set_y(-2.0);
                vec3:set_z(-3.0);

                return vec3:x(), vec3:y(), vec3:z()
            end)")};
    EXPECT_EQ(
        (runner.execute<float, float, float>("set", iris::Vector3(1.1f, 2.2f, 3.3f))),
        std::make_tuple(-1.0f, -2.0f, -3.0f));
}

TEST(lua_script, vector3_to_string)
{
    const iris::Vector3 vec{2.1f, 2.2f, 2.3f};

    std::stringstream strm{};
    strm << vec;

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function vec_tostring(vec)
                return tostring(vec)
            end)")};
    EXPECT_EQ((runner.execute<std::string>("vec_tostring", vec)), strm.str());
}

TEST(lua_script, vector3_equality)
{
    const iris::Vector3 vec1{2.1f, 2.2f, 2.3f};
    const iris::Vector3 vec2{vec1};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function vec_compare(vec1, vec2)
                return vec1 == vec2
            end)")};
    EXPECT_TRUE(runner.execute<bool>("vec_compare", vec1, vec2));
}

TEST(lua_script, vector3_negate)
{
    iris::Vector3 vec{2.1f, 2.2f, 2.3f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function vec_negate(vec)
                return -vec;
            end)")};
    EXPECT_EQ((runner.execute<iris::Vector3>("vec_negate", vec)), -vec);
}

TEST(lua_script, vector3_add)
{
    const iris::Vector3 vec1{2.1f, 2.2f, 2.3f};
    const iris::Vector3 vec2{3.1f, 3.2f, 3.3f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function vec_add(vec1, vec2)
                return vec1 + vec2;
            end)")};
    EXPECT_EQ((runner.execute<iris::Vector3>("vec_add", vec1, vec2)), vec1 + vec2);
}

TEST(lua_script, vector3_sub)
{
    const iris::Vector3 vec1{2.1f, 2.2f, 2.3f};
    const iris::Vector3 vec2{3.1f, 3.2f, 3.3f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function vec_sub(vec1, vec2)
                return vec1 - vec2;
            end)")};
    EXPECT_EQ((runner.execute<iris::Vector3>("vec_sub", vec1, vec2)), vec1 - vec2);
}

TEST(lua_script, vector3_mul)
{
    const iris::Vector3 vec1{2.1f, 2.2f, 2.3f};
    const iris::Vector3 vec2{3.1f, 3.2f, 3.3f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function vec_mul(vec1, vec2)
                return vec1 * vec2;
            end)")};
    EXPECT_EQ((runner.execute<iris::Vector3>("vec_mul", vec1, vec2)), vec1 * vec2);
}

TEST(lua_script, vector3_dot)
{
    iris::Vector3 vec1{1.0f, 3.0f, -5.0f};
    const iris::Vector3 vec2{4.0f, -2.0f, -1.0f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function vec_dot(vec1, vec2)
                return vec1:dot(vec2);
            end)")};
    EXPECT_EQ((runner.execute<float>("vec_dot", vec1, vec2)), vec1.dot(vec2));
}

TEST(lua_script, vector3_cross)
{
    const iris::Vector3 vec1{1.0f, 3.0f, -5.0f};
    const iris::Vector3 vec2{4.0f, -2.0f, -1.0f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function vec_cross(vec1, vec2)
                return vec1:cross(vec2);
            end)")};
    EXPECT_EQ((runner.execute<iris::Vector3>("vec_cross", vec1, vec2)), iris::Vector3::cross(vec1, vec2));
}

TEST(lua_script, vector3_normalise)
{
    const iris::Vector3 vec{1.0f, 3.0f, -5.0f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function vec_normalise(vec)
                vec:normalise()
                return vec
            end)")};
    EXPECT_EQ((runner.execute<iris::Vector3>("vec_normalise", vec)), iris::Vector3::normalise(vec));
}

TEST(lua_script, vector3_magnitude)
{
    const iris::Vector3 vec{1.0f, 3.0f, -5.0f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function vec_magnitude(vec)
                return vec:magnitude()
            end)")};
    EXPECT_EQ((runner.execute<float>("vec_magnitude", vec)), vec.magnitude());
}

TEST(lua_script, vector3_lerp)
{
    const iris::Vector3 vec1{1.0f, 3.0f, -5.0f};
    const iris::Vector3 vec2{4.0f, -2.0f, -1.0f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function vec_lerp(vec1, vec2, amount)
                vec1:lerp(vec2, amount)
                return vec1
            end)")};
    EXPECT_EQ((runner.execute<iris::Vector3>("vec_lerp", vec1, vec2, 0.5f)), iris::Vector3::lerp(vec1, vec2, 0.5f));
}

TEST(lua_script, quaternion_function)
{
    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function quaternion()
                local q = Quaternion(1.0, 2.0, 3.0, 4.0)
                return q:x(), q:y(), q:z(), q:w()
            end)")};
    EXPECT_EQ((runner.execute<float, float, float, float>("quaternion")), std::make_tuple(1.0f, 2.0f, 3.0f, 4.0f));
}

TEST(lua_script, pass_quaternion)
{
    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function split(q)
                return q:x(), q:y(), q:z(), q:w()
            end)")};
    EXPECT_EQ(
        (runner.execute<float, float, float, float>("split", iris::Quaternion(1.1f, 2.2f, 3.3f, 4.4f))),
        std::make_tuple(1.1f, 2.2f, 3.3f, 4.4f));
}

TEST(lua_script, get_quaternion)
{
    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function create_quaternion(x, y, z, w)
                local q = Quaternion(x, y, z, w)
                return q
            end)")};
    EXPECT_EQ(
        (runner.execute<iris::Quaternion>("create_quaternion", 2.1f, 2.2f, 2.3f, 2.4f)),
        iris::Quaternion(2.1f, 2.2f, 2.3f, 2.4f));
}

TEST(lua_script, quaternion_setters)
{
    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function set(q)
                q:set_x(-1.0);
                q:set_y(-2.0);
                q:set_z(-3.0);
                q:set_w(-4.0);

                return q:x(), q:y(), q:z(), q:w()
            end)")};
    EXPECT_EQ(
        (runner.execute<float, float, float, float>("set", iris::Quaternion(1.1f, 2.2f, 3.3f, 4.4f))),
        std::make_tuple(-1.0f, -2.0f, -3.0f, -4.0f));
}

TEST(lua_script, quaternion_to_string)
{
    const iris::Quaternion q{1.0f, 2.0f, 3.0f, 4.0f};

    std::stringstream strm{};
    strm << q;

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function quaternion_tostring(q)
                return tostring(q)
            end)")};
    EXPECT_EQ((runner.execute<std::string>("quaternion_tostring", q)), strm.str());
}

TEST(lua_script, quaternion_equality)
{
    const iris::Quaternion q1{1.0f, 2.0f, 3.0f, 4.0f};
    const iris::Quaternion q2{q1};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function quaternion_compare(q1, q2)
                return q1 == q2
            end)")};
    EXPECT_TRUE(runner.execute<bool>("quaternion_compare", q1, q2));
}

TEST(lua_script, quaternion_negate)
{
    const iris::Quaternion q{1.0f, 2.0f, 3.0f, 4.0f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function quaternion_negate(q)
                return -q
            end)")};
    EXPECT_EQ((runner.execute<iris::Quaternion>("quaternion_negate", q)), -q);
}

TEST(lua_script, quaternion_add)
{
    const iris::Quaternion q1{1.0f, 2.0f, 3.0f, 4.0f};
    const iris::Quaternion q2{1.2f, 2.4f, 3.6f, 4.6f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function quaternion_add(q1, q2)
                return q1 + q2
            end)")};
    EXPECT_EQ((runner.execute<iris::Quaternion>("quaternion_add", q1, q2)), q1 + q2);
}

TEST(lua_script, quaternion_sub)
{
    const iris::Quaternion q1{1.0f, 2.0f, 3.0f, 4.0f};
    const iris::Quaternion q2{1.2f, 2.4f, 3.6f, 4.6f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function quaternion_sub(q1, q2)
                return q1 - q2
            end)")};
    EXPECT_EQ((runner.execute<iris::Quaternion>("quaternion_sub", q1, q2)), q1 - q2);
}

TEST(lua_script, quaternion_mul)
{
    const iris::Quaternion q1{1.0f, 2.0f, 3.0f, 4.0f};
    const iris::Quaternion q2{1.2f, 2.4f, 3.6f, 4.6f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function quaternion_mul(q1, q2)
                return q1 * q2
            end)")};
    EXPECT_EQ((runner.execute<iris::Quaternion>("quaternion_mul", q1, q2)), q1 * q2);
}

TEST(lua_script, quaternion_dot)
{
    const iris::Quaternion q1{1.0f, 2.0f, 3.0f, 4.0f};
    const iris::Quaternion q2{1.2f, 2.4f, 3.6f, 4.6f};

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function quaternion_dot(q1, q2)
                return q1:dot(q2)
            end)")};
    EXPECT_EQ((runner.execute<float>("quaternion_dot", q1, q2)), q1.dot(q2));
}

TEST(lua_script, quaternion_slerp)
{
    const iris::Quaternion q1{1.0f, 2.0f, 3.0f, 4.0f};
    const iris::Quaternion q2{1.2f, 2.4f, 3.6f, 4.6f};
    const auto amount = 0.5f;
    auto expected = q1;
    expected.slerp(q2, amount);

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function quaternion_slerp(q1, q2, amount)
                return q1:slerp(q2, amount)
            end)")};
    EXPECT_EQ((runner.execute<iris::Quaternion>("quaternion_slerp", q1, q2, amount)), expected);
}

TEST(lua_script, quaternion_normalise)
{
    const iris::Quaternion q{1.0f, 2.0f, 3.0f, 4.0f};
    auto expected = q;
    expected.normalise();

    iris::ScriptRunner runner{std::make_unique<iris::LuaScript>(R"(
            function quaternion_normalise(q)
                return q:normalise()
            end)")};
    EXPECT_EQ((runner.execute<iris::Quaternion>("quaternion_normalise", q)), expected);
}
