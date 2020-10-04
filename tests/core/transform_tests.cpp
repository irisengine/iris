#include <gtest/gtest.h>

#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/transform.h"
#include "core/vector3.h"

TEST(transform, empty_ctor)
{
    const iris::Transform transform{};

    const auto expected = iris::Matrix4::make_translate({}) *
                          iris::Matrix4(iris::Quaternion{}) *
                          iris::Matrix4::make_scale({1.0f});

    EXPECT_EQ(transform.matrix(), expected);
}

TEST(transform, transform_ctor)
{
    const iris::Vector3 translate{1.0f, 2.0f, 3.0f};
    const iris::Quaternion rotation{1.0f, 2.0f, 3.0f, 4.0f};
    const iris::Vector3 scale{2.0f};

    const auto expected = iris::Matrix4::make_translate(translate) *
                          iris::Matrix4(rotation) *
                          iris::Matrix4::make_scale(scale);
    const iris::Transform transform{translate, rotation, scale};

    EXPECT_EQ(transform.matrix(), expected);
}

TEST(transform, transform)
{
    const iris::Vector3 translate1{1.0f, 2.0f, 3.0f};
    const iris::Vector3 translate2{1.1f, 2.2f, 3.3f};
    const iris::Quaternion rotation{1.0f, 2.0f, 3.0f, 4.0f};
    const iris::Vector3 scale{2.0f};

    iris::Transform transform{translate1, rotation, scale};
    const auto get_translate1 = transform.translation();
    transform.set_translation(translate2);
    const auto get_translate2 = transform.translation();

    ASSERT_EQ(get_translate1, translate1);
    ASSERT_EQ(get_translate2, translate2);
}

TEST(transform, rotation)
{
    const iris::Vector3 translate{1.0f, 2.0f, 3.0f};
    const iris::Quaternion rotation1{1.0f, 2.0f, 3.0f, 4.0f};
    const iris::Quaternion rotation2{1.1f, 2.2f, 3.3f, 4.4f};
    const iris::Vector3 scale{2.0f};

    iris::Transform transform{translate, rotation1, scale};
    const auto get_rotation1 = transform.rotation();
    transform.set_rotation(rotation2);
    const auto get_rotation2 = transform.rotation();

    ASSERT_EQ(get_rotation1, rotation1);
    ASSERT_EQ(get_rotation2, rotation2);
}

TEST(transform, scale)
{
    const iris::Vector3 translate{1.0f, 2.0f, 3.0f};
    const iris::Quaternion rotation{1.0f, 2.0f, 3.0f, 4.0f};
    const iris::Vector3 scale1{2.0f};
    const iris::Vector3 scale2{2.2f};

    iris::Transform transform{translate, rotation, scale1};
    const auto get_scale1 = transform.scale();
    transform.set_scale(scale2);
    const auto get_scale2 = transform.scale();

    ASSERT_EQ(get_scale1, scale1);
    ASSERT_EQ(get_scale2, scale2);
}

TEST(transform, equality)
{
    const iris::Vector3 translate{1.0f, 2.0f, 3.0f};
    const iris::Quaternion rotation{1.0f, 2.0f, 3.0f, 4.0f};
    const iris::Vector3 scale{2.0f};

    const iris::Transform transform1{translate, rotation, scale};
    const iris::Transform transform2{translate, rotation, scale};

    ASSERT_EQ(transform1, transform2);
}

TEST(transform, inequality)
{
    const iris::Vector3 translate1{1.0f, 2.0f, 3.0f};
    const iris::Vector3 translate2{2.0f, 2.0f, 3.0f};
    const iris::Quaternion rotation{1.0f, 2.0f, 3.0f, 4.0f};
    const iris::Vector3 scale{2.0f};

    const iris::Transform transform1{translate1, rotation, scale};
    const iris::Transform transform2{translate2, rotation, scale};

    ASSERT_NE(translate1, translate2);
}
