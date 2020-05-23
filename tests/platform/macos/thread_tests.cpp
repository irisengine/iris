#include "platform/thread.h"

#include <atomic>
#include <thread>

#include <gtest/gtest.h>

#include "core/exception.h"

TEST(thread, default_constructor)
{
    eng::Thread thrd{ };
    ASSERT_FALSE(thrd.joinable());
}

TEST(thread, function_constructor)
{
    std::atomic<bool> done = false;

    eng::Thread thrd{ [](std::atomic<bool> *done) { *done = true; }, &done };

    ASSERT_TRUE(thrd.joinable());

    thrd.join();

    ASSERT_TRUE(done);
    ASSERT_FALSE(thrd.joinable());
}

TEST(thread, invalid_bind)
{
    eng::Thread thrd{ };
    ASSERT_THROW(
        thrd.bind_to_core(std::thread::hardware_concurrency()),
        eng::Exception);
}
