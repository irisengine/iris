#include <gtest/gtest.h>

#include "chunk.hpp"
#include "exception.hpp"
#include "value.hpp"

TEST(chunk_tests, default_construction)
{
    eng::chunk c{ };

    ASSERT_TRUE(c.code().empty());
    ASSERT_TRUE(c.constants().empty());
    ASSERT_EQ(0u, c.num_objects());
}

TEST(chunk_tests, tmp)
{
    eng::chunk c1{ };
    c1.add_variable("a");

    auto c2 = c1;

    ASSERT_EQ(c1.variables().top(), c2.variables().top());
}

TEST(chunk_tests, add_instruction)
{
    eng::chunk c{ };

    c.add_raw_instruction(0xaa);

    ASSERT_EQ(1u, c.code().size());
    ASSERT_EQ(0xaa, c.code().front());
}

TEST(chunk_tests, add_constant)
{
    eng::chunk c{ };

    ASSERT_EQ(0u, c.add_constant(eng::value{ 1.0f }));
    ASSERT_EQ(1u, c.constants().size());
    ASSERT_EQ(1.0f, c.constants().front().get<float>());
}

TEST(chunk_tests, add_multiple_constants)
{
    eng::chunk c{ };

    ASSERT_EQ(0u, c.add_constant(eng::value{ 1.0f }));
    ASSERT_EQ(1u, c.add_constant(eng::value{ true }));
    ASSERT_EQ(2u, c.constants().size());
    ASSERT_EQ(1.0f, c.constants().front().get<float>());
    ASSERT_EQ(true, c.constants().back().get<bool>());
}

TEST(chunk_tests, add_constant_cached)
{
    eng::chunk c{ };

    ASSERT_EQ(0u, c.add_constant(eng::value{ 1.0f }));
    ASSERT_EQ(0u, c.add_constant(eng::value{ 1.0f }));
    ASSERT_EQ(1u, c.constants().size());
    ASSERT_EQ(1.0f, c.constants().front().get<float>());
}

TEST(chunk_tests, add_variable)
{
    eng::chunk c{ };

    c.add_variable("a");

    ASSERT_EQ(1u, c.variables().top().size());
    ASSERT_EQ(0u, c.variables().top().at("a"));
}

TEST(chunk_tests, add_object)
{
    eng::chunk c{ };

    c.add_object();

    ASSERT_EQ(1u, c.num_objects());
}

TEST(chunk_tests, patch_jump)
{
    eng::chunk c{ };

    c.add_object();
    c.add_raw_instruction(0xaa);
    c.add_raw_instruction(0xbb);
    c.patch_jump(0u, 1u);

    ASSERT_EQ(0x01, c.code().front());
}

TEST(chunk_tests, patch_jump_invalid_jump)
{
    eng::chunk c{ };

    c.add_object();
    c.add_raw_instruction(0xaa);
    ASSERT_THROW(c.patch_jump(10u, 1u), eng::exception);
}

TEST(chunk_tests, patch_jump_invalid_landing)
{
    eng::chunk c{ };

    c.add_object();
    c.add_raw_instruction(0xaa);
    c.add_raw_instruction(0xbb);
    ASSERT_THROW(c.patch_jump(0u, 5u), eng::exception);
}

