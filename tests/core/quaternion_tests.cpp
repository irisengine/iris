#include <gtest/gtest.h>

#include "core/quaternion.h"
#include "core/vector3.h"

TEST(quaternion, basic_constructor)
{
    iris::Quaternion q{};
    ASSERT_EQ(q, iris::Quaternion(0.0f, 0.0f, 0.0f, 1.0f));
}

TEST(quaternion, axis_angle_constructor)
{
    iris::Quaternion q{{1.0f, 0.0f, 0.0f}, 0.5f};
    ASSERT_EQ(q, iris::Quaternion(0.2474039644f, 0.0f, 0.0f, 0.9689124227f));
}

TEST(quaternion, compontent_constructor)
{
    iris::Quaternion q{1.0f, 2.0f, 3.0f, 4.0f};
    ASSERT_EQ(q, iris::Quaternion(1.0f, 2.0f, 3.0f, 4.0f));
}

TEST(quaternion, euler_constructor)
{
    iris::Quaternion q{0.1f, 0.2f, 0.3f};
    std::cout << std::setprecision(10) << q << std::endl;
    ASSERT_EQ(
        q,
        iris::Quaternion(
            0.1435721964f, 0.1060205176f, 0.0342707932f, 0.9833474755f));
}

TEST(quaternion, multiply)
{
    iris::Quaternion q1{{1.0f, 0.0f, 0.0f}, 0.5f};
    iris::Quaternion q2{{0.0f, 0.0f, 1.0f}, 0.2f};
    auto q3 = q1 * q2;

    ASSERT_EQ(
        q3,
        iris::Quaternion(
            0.2461679727f, -0.02469918504f, 0.09672984481f, 0.9640719295f));
}

TEST(quaternion, multiply_assignment)
{
    iris::Quaternion q1{{1.0f, 0.0f, 0.0f}, 0.5f};
    iris::Quaternion q2{{0.0f, 0.0f, 1.0f}, 0.2f};
    q1 *= q2;

    ASSERT_EQ(
        q1,
        iris::Quaternion(
            0.2461679727f, -0.02469918504f, 0.09672984481f, 0.9640719295f));
}

TEST(quaternion, vector_addition)
{
    iris::Quaternion q1{{1.0f, 0.0f, 0.0f}, 0.5f};
    iris::Vector3 v{0.0f, 0.0f, 1.0f};
    auto q2 = q1 + v;

    ASSERT_EQ(
        q2,
        iris::Quaternion(
            0.2474039644f, 0.1237019822f, 0.4844562113f, 0.9689124227f));
}

TEST(quaternion, vector_addition_assignment)
{
    iris::Quaternion q{{1.0f, 0.0f, 0.0f}, 0.5f};
    iris::Vector3 v{0.0f, 0.0f, 1.0f};
    q += v;

    ASSERT_EQ(
        q,
        iris::Quaternion(
            0.2474039644f, 0.1237019822f, 0.4844562113f, 0.9689124227f));
}

TEST(quaternion, scale)
{
    const iris::Quaternion q1{1.0f, 2.0f, 3.0f, 4.0f};
    const auto q2 = q1 * 1.5f;

    ASSERT_EQ(q2, iris::Quaternion(1.5f, 3.0f, 4.5f, 6.0f));
}

TEST(quaternion, scale_assignment)
{
    iris::Quaternion q{1.0f, 2.0f, 3.0f, 4.0f};
    q *= 1.5f;

    ASSERT_EQ(q, iris::Quaternion(1.5f, 3.0f, 4.5f, 6.0f));
}

TEST(quaternion, subtraction)
{
    const iris::Quaternion q1(1.0f, 2.0f, 3.0f, 4.0f);
    const auto q2 = q1 - iris::Quaternion{0.1f, 0.2, 0.3f, 0.4f};

    ASSERT_EQ(q2, iris::Quaternion(0.9f, 1.8f, 2.7f, 3.6f));
}

TEST(quaternion, subtraction_assignment)
{
    iris::Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
    q -= iris::Quaternion{0.1f, 0.2, 0.3f, 0.4f};

    ASSERT_EQ(q, iris::Quaternion(0.9f, 1.8f, 2.7f, 3.6f));
}

TEST(quaternion, addition)
{
    const iris::Quaternion q1(1.0f, 2.0f, 3.0f, 4.0f);
    const auto q2 = q1 + iris::Quaternion{0.1f, 0.2, 0.3f, 0.4f};

    ASSERT_EQ(q2, iris::Quaternion(1.1f, 2.2f, 3.3f, 4.4f));
}

TEST(quaternion, addition_assignment)
{
    iris::Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
    q += iris::Quaternion{0.1f, 0.2, 0.3f, 0.4f};

    ASSERT_EQ(q, iris::Quaternion(1.1f, 2.2f, 3.3f, 4.4f));
}

TEST(quaternion, negate)
{
    iris::Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);

    ASSERT_EQ(-q, iris::Quaternion(-1.0f, -2.0f, -3.0f, -4.0f));
}

TEST(quaternion, dot)
{
    const iris::Quaternion q1(1.1f, 2.2f, 1.1f, 2.2f);
    const iris::Quaternion q2(0.1f, 0.2f, 0.3f, 0.4f);

    ASSERT_EQ(q1.dot(q2), 1.76f);
}

TEST(quaternion, slerp)
{
    iris::Quaternion q1(1.1f, 2.2f, 1.1f, 2.2f);
    const iris::Quaternion q2(0.1f, 0.2f, 0.3f, 0.4f);

    q1.slerp(q2, 0.5f);

    ASSERT_EQ(
        q1,
        iris::Quaternion(
            0.3007528484f, 0.6015056968f, 0.3508783281f, 0.6516311169f));
}

TEST(quaternion, normalise)
{
    iris::Quaternion q{1.0f, 2.0f, 3.0f, 4.0f};
    q.normalise();

    ASSERT_EQ(
        q,
        iris::Quaternion(
            0.1825741827f, 0.3651483655f, 0.5477225184f, 0.730296731f));
}
