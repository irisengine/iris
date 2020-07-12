#include <gtest/gtest.h>

#include "core/quaternion.h"
#include "core/vector3.h"

TEST(quaternion, basic_constructor)
{
    eng::Quaternion q{ };
    ASSERT_EQ(q, eng::Quaternion(0.0f, 0.0f, 0.0f, 1.0f));
}

TEST(quaternion, axis_angle_constructor)
{
    eng::Quaternion q{ { 1.0f, 0.0f, 0.0f }, 0.5f };
    ASSERT_EQ(q, eng::Quaternion(0.2474039644f, 0.0f, 0.0f, 0.9689124227f));
}

TEST(quaternion, compontent_constructor)
{
    eng::Quaternion q{ 1.0f, 2.0f, 3.0f, 4.0f };
    ASSERT_EQ(q, eng::Quaternion(1.0f, 2.0f, 3.0f, 4.0f));
}

TEST(quaternion, multiply)
{
    eng::Quaternion q1{ { 1.0f, 0.0f, 0.0f }, 0.5f };
    eng::Quaternion q2{ { 0.0f, 0.0f, 1.0f }, 0.2f };
    auto q3 = q1 * q2;

    ASSERT_EQ(q3, eng::Quaternion(0.2461679727f, -0.02469918504f, 0.09672984481f, 0.9640719295f));
}

TEST(quaternion, multiply_assignment)
{
    eng::Quaternion q1{ { 1.0f, 0.0f, 0.0f }, 0.5f };
    eng::Quaternion q2{ { 0.0f, 0.0f, 1.0f }, 0.2f };
    q1 *= q2;

    ASSERT_EQ(q1, eng::Quaternion(0.2461679727f, -0.02469918504f, 0.09672984481f, 0.9640719295f));
}

TEST(quaternion, addition)
{
    eng::Quaternion q1{ { 1.0f, 0.0f, 0.0f }, 0.5f };
    eng::Vector3 v{ 0.0f, 0.0f, 1.0f };
    auto q2 = q1 + v;

    ASSERT_EQ(q2, eng::Quaternion(0.2474039644f, 0.1237019822f, 0.4844562113f, 0.9689124227f));
}

TEST(quaternion, addition_assignment)
{
    eng::Quaternion q{ { 1.0f, 0.0f, 0.0f }, 0.5f };
    eng::Vector3 v{ 0.0f, 0.0f, 1.0f };
    q += v;

    ASSERT_EQ(q, eng::Quaternion(0.2474039644f, 0.1237019822f, 0.4844562113f, 0.9689124227f));
}

TEST(quaternion, normalise)
{
    eng::Quaternion q{ 1.0f, 2.0f, 3.0f, 4.0f };
    q.normalise();

    ASSERT_EQ(q, eng::Quaternion(0.1825741827f, 0.3651483655f, 0.5477225184f, 0.730296731f));
}

