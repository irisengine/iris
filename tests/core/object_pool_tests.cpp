#include "core/object_pool.h"

#include <gtest/gtest.h>

#include "core/vector3.h"

TEST(object_pool, single)
{
    iris::ObjectPool<iris::Vector3> pool{};
    const auto *v = pool.next(1.0f, 2.0f, 3.0f);

    ASSERT_EQ(*v, iris::Vector3(1.0f, 2.0f, 3.0f));
}

TEST(object_pool, multiple)
{
    iris::ObjectPool<iris::Vector3> pool{};
    const auto *v1 = pool.next(1.0f, 2.0f, 3.0f);
    const auto *v2 = pool.next(1.1f, 2.1f, 3.1f);
    const auto *v3 = pool.next(1.2f, 2.2f, 3.2f);

    ASSERT_NE(v1, v2);
    ASSERT_NE(v1, v3);
    ASSERT_NE(v2, v3);
    ASSERT_EQ(*v1, iris::Vector3(1.0f, 2.0f, 3.0f));
    ASSERT_EQ(*v2, iris::Vector3(1.1f, 2.1f, 3.1f));
    ASSERT_EQ(*v3, iris::Vector3(1.2f, 2.2f, 3.2f));
}

TEST(object_pool, drained_pool)
{
    iris::ObjectPool<int, 3> pool{};

    pool.next();
    pool.next();
    pool.next();
    ASSERT_THROW(pool.next(), iris::Exception);
}

TEST(object_pool, release)
{
    iris::ObjectPool<int> pool{};

    auto i1 = pool.next();
    pool.release(i1);
    auto i2 = pool.next();

    ASSERT_EQ(i1, i2);
}
