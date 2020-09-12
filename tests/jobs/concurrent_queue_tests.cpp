#include <numeric>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "jobs/concurrent_queue.h"

TEST(concurrent_queue, constructor)
{
    iris::ConcurrentQueue<int> q;
    ASSERT_TRUE(q.empty());
}

TEST(concurrent_queue, enqueue)
{
    iris::ConcurrentQueue<int> q;
    q.enqueue(1);

    ASSERT_FALSE(q.empty());
}

TEST(concurrent_queue, try_dequeue)
{
    iris::ConcurrentQueue<int> q;
    q.enqueue(1);
    int value = 0;

    ASSERT_TRUE(q.try_dequeue(value));
    ASSERT_TRUE(q.empty());
    ASSERT_EQ(value, 1);
}

TEST(concurrent_queue, enqueue_thread_safe)
{
    static constexpr auto value_count = 10000;
    iris::ConcurrentQueue<int> q;
    std::vector<int> values(value_count);
    std::iota(std::begin(values), std::end(values), 0);

    const auto worker = [&q, &values](int start) {
        for (int i = start; i < start + (value_count / 4); i++)
        {
            q.enqueue(values[i]);
        }
    };

    std::thread thrd1{worker, (value_count / 4) * 0};
    std::thread thrd2{worker, (value_count / 4) * 1};
    std::thread thrd3{worker, (value_count / 4) * 2};
    std::thread thrd4{worker, (value_count / 4) * 3};

    thrd1.join();
    thrd2.join();
    thrd3.join();
    thrd4.join();

    std::vector<int> popped;

    for (int i = 0; i < value_count; ++i)
    {
        int element = 0;
        ASSERT_TRUE(q.try_dequeue(element));
        popped.emplace_back(element);
    }

    std::sort(std::begin(popped), std::end(popped));
    ASSERT_EQ(popped, values);
}
