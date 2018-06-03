#include <string>

#include <gtest/gtest.h>

#include "exception.hpp"
#include "function.hpp"
#include "value.hpp"

using std::string_literals::operator""s;

TEST(value_tests, default_value)
{
    eng::value v{ };
    ASSERT_TRUE(v.is_type<float>());
    ASSERT_EQ(0.0f, v.get<float>());
}

TEST(value_tests, is_type)
{
    eng::object obj{ };

    ASSERT_TRUE(eng::value{ 0.0f }.is_type<float>());
    ASSERT_TRUE(eng::value{ "hello"s }.is_type<std::string>());
    ASSERT_TRUE(eng::value{ true }.is_type<bool>());
    ASSERT_TRUE(eng::value{ &obj }.is_type<eng::object*>());
    ASSERT_TRUE(eng::value{ eng::function(nullptr, nullptr, 0u) }.is_type<eng::function>());
}

TEST(value_tests, is_same)
{
    eng::value num1{ 0.0f };
    eng::value num2{ 1.0f };
    eng::value str1{ "hello"s };
    eng::value str2{ "yo"s };
    eng::value bool1{ true };
    eng::value bool2{ false };
    eng::object obj_data1{ { eng::value{ 0.1f }, eng::value{ true } } };
    eng::object obj_data2{ { eng::value{ false }, eng::value{ "yo"s } } };
    eng::value obj1{ &obj_data1 };
    eng::value obj2{ &obj_data2 };
    eng::value func1{ eng::function { nullptr, nullptr, 0u } };
    eng::value func2{ eng::function { nullptr, nullptr, 0u } };

    ASSERT_TRUE(num1.is_same(num2));
    ASSERT_TRUE(str1.is_same(str2));
    ASSERT_TRUE(bool1.is_same(bool2));
    ASSERT_TRUE(obj1.is_same(obj2));
    ASSERT_TRUE(func1.is_same(func2));
}

TEST(value_tests, is_not_same)
{
    eng::value num{ 0.0f };
    eng::value str{ "hello"s };
    eng::value bool_val{ true };
    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };
    eng::value obj{ &obj_data };
    eng::value func{ eng::function { nullptr, nullptr, 0u } };

    ASSERT_FALSE(num.is_same(str));
    ASSERT_FALSE(num.is_same(bool_val));
    ASSERT_FALSE(num.is_same(obj));
    ASSERT_FALSE(num.is_same(func));

    ASSERT_FALSE(str.is_same(num));
    ASSERT_FALSE(str.is_same(bool_val));
    ASSERT_FALSE(str.is_same(obj));
    ASSERT_FALSE(str.is_same(func));

    ASSERT_FALSE(bool_val.is_same(str));
    ASSERT_FALSE(bool_val.is_same(num));
    ASSERT_FALSE(bool_val.is_same(obj));
    ASSERT_FALSE(bool_val.is_same(func));

    ASSERT_FALSE(obj.is_same(str));
    ASSERT_FALSE(obj.is_same(bool_val));
    ASSERT_FALSE(obj.is_same(num));
    ASSERT_FALSE(obj.is_same(func));

    ASSERT_FALSE(func.is_same(str));
    ASSERT_FALSE(func.is_same(bool_val));
    ASSERT_FALSE(func.is_same(num));
    ASSERT_FALSE(func.is_same(obj));
}

TEST(value_tests, get)
{
    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };
    eng::value obj{ &obj_data };

    std::vector<std::uint8_t> code{ 0xaa };
    std::vector<eng::location_data> location{ { 1u, 2u } };
    eng::function func{ &code, &location, 0u };

    ASSERT_EQ(1.0f, eng::value{ 1.0f }.get<float>());
    ASSERT_EQ("hello"s, eng::value{ "hello"s }.get<std::string>());
    ASSERT_EQ(true, eng::value{ true }.get<bool>());
    ASSERT_EQ(&obj_data, obj.get<eng::object*>());
    ASSERT_EQ(func, eng::value{ func }.get<eng::function>());
}

TEST(value_tests, get_incorrect_type)
{
    eng::value num{ 0.0f };
    eng::value str{ "hello"s };
    eng::value bool_val{ true };
    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };
    eng::value obj{ &obj_data };
    eng::value func{ eng::function{ nullptr, nullptr, 0u } };

    ASSERT_THROW(num.get<std::string>(), eng::exception);
    ASSERT_THROW(num.get<bool>(), eng::exception);
    ASSERT_THROW(num.get<eng::object*>(), eng::exception);
    ASSERT_THROW(num.get<eng::function>(), eng::exception);

    ASSERT_THROW(str.get<float>(), eng::exception);
    ASSERT_THROW(str.get<bool>(), eng::exception);
    ASSERT_THROW(str.get<eng::object*>(), eng::exception);
    ASSERT_THROW(str.get<eng::function>(), eng::exception);

    ASSERT_THROW(bool_val.get<std::string>(), eng::exception);
    ASSERT_THROW(bool_val.get<float>(), eng::exception);
    ASSERT_THROW(bool_val.get<eng::object*>(), eng::exception);
    ASSERT_THROW(bool_val.get<eng::function>(), eng::exception);

    ASSERT_THROW(obj.get<std::string>(), eng::exception);
    ASSERT_THROW(obj.get<bool>(), eng::exception);
    ASSERT_THROW(obj.get<float>(), eng::exception);
    ASSERT_THROW(obj.get<eng::function>(), eng::exception);

    ASSERT_THROW(func.get<std::string>(), eng::exception);
    ASSERT_THROW(func.get<bool>(), eng::exception);
    ASSERT_THROW(func.get<float>(), eng::exception);
    ASSERT_THROW(func.get<eng::object*>(), eng::exception);
}

TEST(value_tests, get_type)
{
    eng::value num{ 0.0f };
    eng::value str{ "hello"s };
    eng::value bool_val{ true };
    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };
    eng::value obj{ &obj_data };

    ASSERT_THROW(num.get<std::string>(), eng::exception);
    ASSERT_THROW(num.get<bool>(), eng::exception);
    ASSERT_THROW(num.get<eng::object*>(), eng::exception);

    ASSERT_THROW(str.get<float>(), eng::exception);
    ASSERT_THROW(str.get<bool>(), eng::exception);
    ASSERT_THROW(str.get<eng::object*>(), eng::exception);

    ASSERT_THROW(bool_val.get<std::string>(), eng::exception);
    ASSERT_THROW(bool_val.get<float>(), eng::exception);
    ASSERT_THROW(bool_val.get<eng::object*>(), eng::exception);

    ASSERT_THROW(obj.get<std::string>(), eng::exception);
    ASSERT_THROW(obj.get<bool>(), eng::exception);
    ASSERT_THROW(obj.get<float>(), eng::exception);
}

TEST(value_tests, set)
{
    eng::value val{ };

    val.set(0.0f);
    ASSERT_TRUE(val.is_type<float>());

    val.set(true);
    ASSERT_TRUE(val.is_type<bool>());

    val.set("hi"s);
    ASSERT_TRUE(val.is_type<std::string>());

    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };
    val.set(&obj_data);
    ASSERT_TRUE(val.is_type<eng::object*>());

    val.set(eng::function{ nullptr, nullptr, 0u });
    ASSERT_TRUE(val.is_type<eng::function>());
}

TEST(value_tests, equals)
{
    std::vector<std::uint8_t> code1{ 0xaa };
    std::vector<eng::location_data> location1{ { 1u, 2u } };
    eng::function func1{ &code1, &location1, 0u };

    auto code2 = code1;
    auto location2 = location1;
    eng::function func2{ &code2, &location2, 0u };

    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };

    ASSERT_TRUE(eng::value{ 1.0f } == eng::value{ 1.0f });
    ASSERT_TRUE(eng::value{ "hello"s } == eng::value{ "hello"s });
    ASSERT_TRUE(eng::value{ true } == eng::value{ true });
    ASSERT_TRUE(eng::value{ &obj_data } == eng::value{ &obj_data });
    ASSERT_TRUE(eng::value{ func1 } == eng::value{ func2 });
}

TEST(value_tests, not_equals)
{
    std::vector<std::uint8_t> code1{ 0xaa };
    std::vector<eng::location_data> location1{ { 1u, 2u } };
    eng::function func1{ &code1, &location1, 0u };

    std::vector<std::uint8_t> code2{ 0xaa, 0xbb };
    std::vector<eng::location_data> location2{ { 2u, 3u } };
    eng::function func2{ &code2, &location2, 0u };

    eng::object obj_data1{ { eng::value{ 0.1f }, eng::value{ true } } };
    eng::object obj_data2{ { eng::value{ 0.1f }, eng::value{ true } } };

    ASSERT_TRUE(eng::value{ 1.0f } != eng::value{ 2.0f });
    ASSERT_TRUE(eng::value{ "hello"s } != eng::value{ "yo"s });
    ASSERT_TRUE(eng::value{ true } != eng::value{ false });
    ASSERT_TRUE(eng::value{ &obj_data1 } != eng::value{ &obj_data2 });
    ASSERT_TRUE(eng::value{ func1 } != eng::value{ func2 });
}

TEST(value_tests, less)
{
    ASSERT_TRUE(eng::value{ 1.0f } < eng::value{ 2.0f });
    ASSERT_FALSE(eng::value{ 1.0f } < eng::value{ 1.0f });
}

TEST(value_tests, less_equal)
{
    ASSERT_TRUE(eng::value{ 1.0f } <= eng::value{ 2.0f });
    ASSERT_TRUE(eng::value{ 1.0f } <= eng::value{ 1.0f });
}

TEST(value_tests, more)
{
    ASSERT_TRUE(eng::value{ 2.0f } > eng::value{ 1.0f });
    ASSERT_FALSE(eng::value{ 1.0f } > eng::value{ 1.0f });
}

TEST(value_tests, more_equal)
{
    ASSERT_TRUE(eng::value{ 2.0f } >= eng::value{ 1.0f });
    ASSERT_TRUE(eng::value{ 1.0f } >= eng::value{ 1.0f });
}

TEST(value_tests, add_num)
{
    const auto v = eng::value{ 1.0f } + eng::value{ 2.0f };
    ASSERT_EQ(3.0f, v.get<float>());
}

TEST(value_tests, add_string)
{
    const auto v = eng::value{ "hello "s } + eng::value{ "world"s };
    ASSERT_EQ("hello world", v.get<std::string>());
}

TEST(value_tests, add_type_mismatch)
{
    ASSERT_THROW(eng::value{ "hello "s } + eng::value{ 1.0f }, eng::exception);
}

TEST(value_tests, add_incorrect_type)
{
    std::vector<std::uint8_t> code{ 0xaa };
    std::vector<eng::location_data> location{ { 1u, 2u } };
    eng::function func{ &code, &location, 0u };
    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };

    ASSERT_THROW(eng::value{ true } + eng::value{ true }, eng::exception);
    ASSERT_THROW(eng::value{ &obj_data } + eng::value{ &obj_data }, eng::exception);
    ASSERT_THROW(eng::value{ func } + eng::value{ func }, eng::exception);
}

TEST(value_tests, add_assign_num)
{
    auto v = eng::value{ 1.0f };
    v += eng::value{ 2.0f };
    ASSERT_EQ(3.0f, v.get<float>());
}

TEST(value_tests, add_assign_string)
{
    auto v = eng::value{ "hello "s };
    v += eng::value{ "world"s };
    ASSERT_EQ("hello world", v.get<std::string>());
}

TEST(value_tests, add_assign_type_mismatch)
{
    ASSERT_THROW(eng::value{ "hello "s } += eng::value{ 1.0f }, eng::exception);
}

TEST(value_tests, add_assign_incorrect_type)
{
    std::vector<std::uint8_t> code{ 0xaa };
    std::vector<eng::location_data> location{ { 1u, 2u } };
    eng::function func{ &code, &location, 0u };
    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };

    ASSERT_THROW(eng::value{ true } += eng::value{ true }, eng::exception);
    ASSERT_THROW(eng::value{ &obj_data } += eng::value{ &obj_data }, eng::exception);
    ASSERT_THROW(eng::value{ func } += eng::value{ func }, eng::exception);
}

TEST(value_tests, sub_num)
{
    const auto v = eng::value{ 1.0f } - eng::value{ 2.0f };
    ASSERT_EQ(-1.0f, v.get<float>());
}

TEST(value_tests, sub_incorrect_type)
{
    std::vector<std::uint8_t> code{ 0xaa };
    std::vector<eng::location_data> location{ { 1u, 2u } };
    eng::function func{ &code, &location, 0u };
    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };

    ASSERT_THROW(eng::value{ "a"s } - eng::value{ "b"s }, eng::exception);
    ASSERT_THROW(eng::value{ true } - eng::value{ true }, eng::exception);
    ASSERT_THROW(eng::value{ &obj_data } - eng::value{ &obj_data }, eng::exception);
    ASSERT_THROW(eng::value{ func } - eng::value{ func }, eng::exception);
}

TEST(value_tests, sub_assign_num)
{
    auto v = eng::value{ 1.0f };
    v -= eng::value{ 2.0f };
    ASSERT_EQ(-1.0f, v.get<float>());
}

TEST(value_tests, sub_assign_incorrect_type)
{
    std::vector<std::uint8_t> code{ 0xaa };
    std::vector<eng::location_data> location{ { 1u, 2u } };
    eng::function func{ &code, &location, 0u };
    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };

    ASSERT_THROW(eng::value{ "a"s } -= eng::value{ "b"s }, eng::exception);
    ASSERT_THROW(eng::value{ true } -= eng::value{ true }, eng::exception);
    ASSERT_THROW(eng::value{ &obj_data } -= eng::value{ &obj_data }, eng::exception);
    ASSERT_THROW(eng::value{ func } -= eng::value{ func }, eng::exception);
}

TEST(value_tests, mul_num)
{
    const auto v = eng::value{ 2.0f } * eng::value{ 2.0f };
    ASSERT_EQ(4.0f, v.get<float>());
}

TEST(value_tests, mul_incorrect_type)
{
    std::vector<std::uint8_t> code{ 0xaa };
    std::vector<eng::location_data> location{ { 1u, 2u } };
    eng::function func{ &code, &location, 0u };
    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };

    ASSERT_THROW(eng::value{ "a"s } * eng::value{ "b"s }, eng::exception);
    ASSERT_THROW(eng::value{ true } * eng::value{ true }, eng::exception);
    ASSERT_THROW(eng::value{ &obj_data } * eng::value{ &obj_data }, eng::exception);
    ASSERT_THROW(eng::value{ func } * eng::value{ func }, eng::exception);
}

TEST(value_tests, mul_assign_num)
{
    auto v = eng::value{ 2.0f };
    v *= eng::value{ 2.0f };
    ASSERT_EQ(4.0f, v.get<float>());
}

TEST(value_tests, mul_assign_incorrect_type)
{
    std::vector<std::uint8_t> code{ 0xaa };
    std::vector<eng::location_data> location{ { 1u, 2u } };
    eng::function func{ &code, &location, 0u };
    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };

    ASSERT_THROW(eng::value{ "a"s } *= eng::value{ "b"s }, eng::exception);
    ASSERT_THROW(eng::value{ true } *= eng::value{ true }, eng::exception);
    ASSERT_THROW(eng::value{ &obj_data } *= eng::value{ &obj_data }, eng::exception);
    ASSERT_THROW(eng::value{ func } *= eng::value{ func }, eng::exception);
}

TEST(value_tests, div_num)
{
    const auto v = eng::value{ 1.0f } / eng::value{ 2.0f };
    ASSERT_EQ(0.5f, v.get<float>());
}

TEST(value_tests, div_incorrect_type)
{
    std::vector<std::uint8_t> code{ 0xaa };
    std::vector<eng::location_data> location{ { 1u, 2u } };
    eng::function func{ &code, &location, 0u };
    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };

    ASSERT_THROW(eng::value{ "a"s } / eng::value{ "b"s }, eng::exception);
    ASSERT_THROW(eng::value{ true } / eng::value{ true }, eng::exception);
    ASSERT_THROW(eng::value{ &obj_data } / eng::value{ &obj_data }, eng::exception);
    ASSERT_THROW(eng::value{ func } / eng::value{ func }, eng::exception);
}

TEST(value_tests, div_assign_num)
{
    auto v = eng::value{ 1.0f };
    v /= eng::value{ 2.0f };
    ASSERT_EQ(0.5f, v.get<float>());
}

TEST(value_tests, div_assign_incorrect_type)
{
    std::vector<std::uint8_t> code{ 0xaa };
    std::vector<eng::location_data> location{ { 1u, 2u } };
    eng::function func{ &code, &location, 0u };
    eng::object obj_data{ { eng::value{ 0.1f }, eng::value{ true } } };

    ASSERT_THROW(eng::value{ "a"s } /= eng::value{ "b"s }, eng::exception);
    ASSERT_THROW(eng::value{ true } /= eng::value{ true }, eng::exception);
    ASSERT_THROW(eng::value{ &obj_data } /= eng::value{ &obj_data }, eng::exception);
    ASSERT_THROW(eng::value{ func } /= eng::value{ func }, eng::exception);
}

