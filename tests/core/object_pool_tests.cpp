#include "core/object_pool.h"

#include <gtest/gtest.h>

#include "core/thread.h"
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

TEST(object_pool, multiple_of_order)
{
    iris::ObjectPool<iris::Vector3> pool{};
    const auto *v1 = pool.next(1.0f, 2.0f, 3.0f);
    const auto *v2 = pool.next(1.1f, 2.1f, 3.1f);
    pool.release(v1);
    const auto *v3 = pool.next(1.2f, 2.2f, 3.2f);

    ASSERT_NE(v2, v3);
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

TEST(object_pool, thread_safe_next)
{
    iris::ObjectPool<iris::Vector3> pool{};
    std::vector<iris::Vector3> vec1;
    std::vector<iris::Vector3> vec2;
    std::atomic<bool> start = false;

    iris::Thread t1 = [&start, &vec1, &pool]()
    {
        while (!start)
        {
        }

        for (auto i = 0u; i < 500u; ++i)
        {
            vec1.emplace_back(*pool.next(1.0f, 1.0f, 1.0f));
        }
    };

    iris::Thread t2 = [&start, &vec2, &pool]()
    {
        while (!start)
        {
        }

        for (auto i = 0u; i < 500u; ++i)
        {
            vec2.emplace_back(*pool.next(2.0f, 2.0f, 2.0f));
        }
    };

    start = true;
    t1.join();
    t2.join();

    ASSERT_EQ(vec1.size(), 500u);
    ASSERT_EQ(vec2.size(), 500u);
    ASSERT_TRUE(std::all_of(
        std::cbegin(vec1), std::cend(vec1), [&vec1](const auto &element) { return vec1.front() == element; }));
    ASSERT_TRUE(std::all_of(
        std::cbegin(vec2), std::cend(vec2), [&vec2](const auto &element) { return vec2.front() == element; }));
}
