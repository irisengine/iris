#include <gtest/gtest.h>

#include "core/vector3.h"

TEST(vector3, default_constructor)
{
    iris::Vector3 v{};

    ASSERT_EQ(v.x, 0.0f);
    ASSERT_EQ(v.y, 0.0f);
    ASSERT_EQ(v.x, 0.0f);
}

TEST(vector3, single_value_constructor)
{
    iris::Vector3 v{1.1f};

    ASSERT_EQ(v.x, 1.1f);
    ASSERT_EQ(v.y, 1.1f);
    ASSERT_EQ(v.z, 1.1f);
}

TEST(vector3, multi_value_constructor)
{
    iris::Vector3 v{1.1f, 2.2f, 3.3f};

    ASSERT_EQ(v.x, 1.1f);
    ASSERT_EQ(v.y, 2.2f);
    ASSERT_EQ(v.z, 3.3f);
}

TEST(vector3, equality)
{
    iris::Vector3 v1{1.0f, 2.0f, 3.0f};
    iris::Vector3 v2{1.0f, 2.0f, 3.0f};

    ASSERT_EQ(v1, v2);
}

TEST(vector3, inequality)
{
    iris::Vector3 v1{1.0f, 2.0f, 3.0f};
    iris::Vector3 v2{1.1f, 2.2f, 3.2f};

    ASSERT_NE(v1, v2);
}

TEST(vector3, scale)
{
    iris::Vector3 v1{1.1f, 2.2f, 3.3f};
    auto v2 = v1 * 2.0f;

    ASSERT_EQ(v2, iris::Vector3(2.2f, 4.4f, 6.6f));
}

TEST(vector3, scale_assignment)
{
    iris::Vector3 v{1.1f, 2.2f, 3.3f};
    v *= 2.0f;

    ASSERT_EQ(v, iris::Vector3(2.2f, 4.4f, 6.6f));
}

TEST(vector3, add)
{
    iris::Vector3 v1{1.1f, 2.2f, 3.3f};
    auto v2 = v1 + iris::Vector3{1.0f, 2.0f, 3.0f};

    ASSERT_EQ(v2, iris::Vector3(2.1f, 4.2f, 6.3f));
}

TEST(vector3, add_assignment)
{
    iris::Vector3 v{1.1f, 2.2f, 3.3f};
    v += iris::Vector3{1.0f, 2.0f, 3.0f};

    ASSERT_EQ(v, iris::Vector3(2.1f, 4.2f, 6.3f));
}

TEST(vector3, subtract)
{
    iris::Vector3 v1{1.1f, 2.2f, 3.3f};
    auto v2 = v1 - iris::Vector3{1.0f, 2.0f, 3.0f};

    ASSERT_EQ(v2, iris::Vector3(0.1f, 0.2f, 0.3f));
}

TEST(vector3, subtract_assignment)
{
    iris::Vector3 v{1.1f, 2.2f, 3.3f};
    v -= iris::Vector3{1.0f, 2.0f, 3.0f};

    ASSERT_EQ(v, iris::Vector3(0.1f, 0.2f, 0.3f));
}

TEST(vector3, multiply)
{
    iris::Vector3 v1{1.1f, 2.2f, 3.3f};
    auto v2 = v1 * iris::Vector3{1.0f, 2.0f, 3.0f};

    ASSERT_EQ(v2, iris::Vector3(1.1f, 4.4f, 9.9f));
}

TEST(vector3, multiply_assignment)
{
    iris::Vector3 v{1.1f, 2.2f, 3.3f};
    v *= iris::Vector3{1.0f, 2.0f, 3.0f};

    ASSERT_EQ(v, iris::Vector3(1.1f, 4.4f, 9.9f));
}

TEST(vector3, negate)
{
    iris::Vector3 v{1.1f, 2.2f, 3.3f};

    ASSERT_EQ(-v, iris::Vector3(-1.1f, -2.2f, -3.3f));
}

TEST(vector3, dot)
{
    ASSERT_EQ(
        iris::Vector3(1.0f, 3.0f, -5.0f).dot(iris::Vector3(4.0f, -2.0f, -1.0f)),
        3.0f);
}

TEST(vector3, cross)
{
    ASSERT_EQ(
        iris::Vector3(2.0f, 3.0f, 4.0f).cross(iris::Vector3(5.0f, 6.0f, 7.0f)),
        iris::Vector3(-3.0f, 6.0f, -3.0f));
}

TEST(vector3, cross_static)
{
    ASSERT_EQ(
        iris::Vector3::cross({2.0f, 3.0f, 4.0f}, {5.0f, 6.0f, 7.0f}),
        iris::Vector3(-3.0f, 6.0f, -3.0f));
}

TEST(vector3, normalise)
{
    iris::Vector3 v{1.0f, 2.0f, 3.0f};
    v.normalise();

    ASSERT_EQ(v, iris::Vector3(0.2672612f, 0.5345225f, 0.8017837f));
}

TEST(vector3, normalise_zero_vector3)
{
    iris::Vector3 v{};
    v.normalise();

    ASSERT_EQ(v, iris::Vector3());
}

TEST(vector3, normalise_stataic)
{
    ASSERT_EQ(
        iris::Vector3::normalise(iris::Vector3(1.0f, 2.0f, 3.0f)),
        iris::Vector3(0.2672612f, 0.5345225f, 0.8017837f));
}

TEST(vector3, magnitude)
{
    iris::Vector3 v{1.0f, 2.0f, 3.0f};

    ASSERT_FLOAT_EQ(v.magnitude(), 3.7416574954986572265625f);
}

TEST(vector3, lerp)
{
    iris::Vector3 vec(0.0f, 0.0f, 0.f);
    iris::Vector3 end(1.0f, 1.0f, 1.0f);

    vec.lerp(end, 0.5f);

    ASSERT_EQ(vec, iris::Vector3(0.5f, 0.5f, 0.5f));
}