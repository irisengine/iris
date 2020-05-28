#include <gtest/gtest.h>

#include "core/vector3.h"

TEST(vector3, default_constructor)
{
    eng::Vector3 v{ };

    ASSERT_EQ(v.x, 0.0f);
    ASSERT_EQ(v.y, 0.0f);
    ASSERT_EQ(v.x, 0.0f);
}

TEST(vector3, constructor)
{
    eng::Vector3 v{ 1.1f, 2.2f, 3.3f };

    ASSERT_EQ(v.x, 1.1f);
    ASSERT_EQ(v.y, 2.2f);
    ASSERT_EQ(v.z, 3.3f);
}

TEST(vector3, equality)
{
    eng::Vector3 v1{ 1.0f, 2.0f, 3.0f };
    eng::Vector3 v2{ 1.0f, 2.0f, 3.0f };

    ASSERT_EQ(v1, v2);
}

TEST(vector3, inequality)
{
    eng::Vector3 v1{ 1.0f, 2.0f, 3.0f };
    eng::Vector3 v2{ 1.1f, 2.2f, 3.2f };

    ASSERT_NE(v1, v2);
}

TEST(vector3, scale)
{
    eng::Vector3 v1{ 1.1f, 2.2f, 3.3f };
    auto v2 = v1 * 2.0f;

    ASSERT_EQ(v2, eng::Vector3(2.2f, 4.4f, 6.6f));
}

TEST(vector3, scale_assignment)
{
    eng::Vector3 v{ 1.1f, 2.2f, 3.3f };
    v *= 2.0f;

    ASSERT_EQ(v, eng::Vector3(2.2f, 4.4f, 6.6f));
}

TEST(vector3, add)
{
    eng::Vector3 v1{ 1.1f, 2.2f, 3.3f };
    auto v2 = v1 + eng::Vector3{ 1.0f, 2.0f, 3.0f };

    ASSERT_EQ(v2, eng::Vector3(2.1f, 4.2f, 6.3f));
}

TEST(vector3, add_assignment)
{
    eng::Vector3 v{ 1.1f, 2.2f, 3.3f };
    v += eng::Vector3{ 1.0f, 2.0f, 3.0f };

    ASSERT_EQ(v, eng::Vector3(2.1f, 4.2f, 6.3f));
}

TEST(vector3, subtract)
{
    eng::Vector3 v1{ 1.1f, 2.2f, 3.3f };
    auto v2 = v1 - eng::Vector3{ 1.0f, 2.0f, 3.0f };

    ASSERT_EQ(v2, eng::Vector3(0.1f, 0.2f, 0.3f));
}

TEST(vector3, subtract_assignment)
{
    eng::Vector3 v{ 1.1f, 2.2f, 3.3f };
    v -= eng::Vector3{ 1.0f, 2.0f, 3.0f };

    ASSERT_EQ(v, eng::Vector3(0.1f, 0.2f, 0.3f));
}

TEST(vector3, multiply)
{
    eng::Vector3 v1{ 1.1f, 2.2f, 3.3f };
    auto v2 = v1 * eng::Vector3{ 1.0f, 2.0f, 3.0f };

    ASSERT_EQ(v2, eng::Vector3(1.1f, 4.4f, 9.9f));
}

TEST(vector3, multiply_assignment)
{
    eng::Vector3 v{ 1.1f, 2.2f, 3.3f };
    v *= eng::Vector3{ 1.0f, 2.0f, 3.0f };

    ASSERT_EQ(v, eng::Vector3(1.1f, 4.4f, 9.9f));
}

TEST(vector3, negate)
{
    eng::Vector3 v{ 1.1f, 2.2f, 3.3f };

    ASSERT_EQ(-v, eng::Vector3(-1.1f, -2.2f, -3.3f));
}

TEST(vector3, dot)
{
    ASSERT_EQ(
        eng::Vector3(1.0f, 3.0f, -5.0f).dot(eng::Vector3(4.0f, -2.0f, -1.0f)),
        3.0f);
}

TEST(vector3, cross)
{
    ASSERT_EQ(
        eng::Vector3(2.0f, 3.0f, 4.0f).cross(eng::Vector3(5.0f, 6.0f, 7.0f)),
        eng::Vector3(-3.0f, 6.0f, -3.0f));
}

TEST(vector3, cross_static)
{
    ASSERT_EQ(
        eng::Vector3::cross({ 2.0f, 3.0f, 4.0f }, { 5.0f, 6.0f, 7.0f }),
        eng::Vector3(-3.0f, 6.0f, -3.0f));
}

TEST(vector3, normalise)
{
    eng::Vector3 v{ 1.0f, 2.0f, 3.0f };
    v.normalise();

    ASSERT_EQ(v, eng::Vector3(0.2672612f, 0.5345225f, 0.8017837f));
}

TEST(vector3, normalise_zero_vector3)
{
    eng::Vector3 v{ };
    v.normalise();

    ASSERT_EQ(v, eng::Vector3());
}

TEST(vector3, normalise_stataic)
{
    ASSERT_EQ(
        eng::Vector3::normalise(eng::Vector3(1.0f, 2.0f, 3.0f)),
        eng::Vector3(0.2672612f, 0.5345225f, 0.8017837f));
}

TEST(vector3, magnitude)
{
    eng::Vector3 v{ 1.0f, 2.0f, 3.0f };

    ASSERT_FLOAT_EQ(v.magnitude(), 3.7416574954986572265625f);
}

