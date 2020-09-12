#include <gtest/gtest.h>

#include <mutex>
#include <thread>

#include "core/exception.h"
#include "jobs/fiber/counter.h"

TEST(counter, constructor)
{
    iris::Counter ctr(3);

    ASSERT_EQ(static_cast<int>(ctr), 3);
}

TEST(counter, prefix_decrement)
{
    iris::Counter ctr(3);
    --ctr;

    ASSERT_EQ(static_cast<int>(ctr), 2);
}

TEST(counter, postfix_decrement)
{
    iris::Counter ctr(3);
    ctr--;

    ASSERT_EQ(static_cast<int>(ctr), 2);
}

TEST(counter, thread_safe)
{
    static constexpr auto value = 10000;
    iris::Counter ctr(value);

    auto dec_thread = [&ctr](){
        for(auto i = 0; i < value / 4; ++i)
        {
            --ctr;
        }
    };

    std::thread thrd1 { dec_thread };
    std::thread thrd2 { dec_thread };
    std::thread thrd3 { dec_thread };
    std::thread thrd4 { dec_thread };

    thrd1.join();
    thrd2.join();
    thrd3.join();
    thrd4.join();

    ASSERT_EQ(static_cast<int>(ctr), 0);
}

