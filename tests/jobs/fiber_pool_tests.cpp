#include <gtest/gtest.h>

#include "core/exception.h"
#include "jobs/fiber/fiber_pool.h"

TEST(fiber_pool, constructor)
{
    eng::FiberPool<4u> pool;
    ASSERT_EQ(pool.capacity(), 4u);
}

TEST(fiber_pool, capacity_grows)
{
    eng::FiberPool<4u> pool;
    
    pool.next();
    pool.next();
    pool.next();
    pool.next();

    ASSERT_EQ(pool.capacity(), 8u);
}

TEST(fiber_pool, next_release)
{
    eng::FiberPool<4u> pool;

    auto fib1 = pool.next();
    auto fib2 = pool.next();
    auto fib3 = pool.next();
    auto fib4 = pool.next();

    pool.release(fib1);
    pool.release(fib2);
    pool.release(fib3);
    pool.release(fib4);

    ASSERT_EQ(pool.next(), fib4);
    ASSERT_EQ(pool.next(), fib3);
    ASSERT_EQ(pool.next(), fib2);
    ASSERT_EQ(pool.next(), fib1);
}

TEST(fiber_pool, throw_on_extra_release)
{
    eng::Fiber fiber;
    eng::FiberPool<4u> pool;
    ASSERT_THROW(pool.release(&fiber), eng::Exception);
}

