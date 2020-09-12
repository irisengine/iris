#include <atomic>
#include <stdexcept>

#include <jobs/job_system.h>

#include <gtest/gtest.h>

TEST(jobs, add_jobs_single)
{
    iris::JobSystem js;
    std::atomic<bool> done = false;

    js.add_jobs({
        [&done](){ done = true; }
    });

    while(!done){}

    ASSERT_TRUE(done);
}

TEST(jobs, add_jobs_multiple)
{
    iris::JobSystem js;
    std::atomic<int> counter = 0;

    js.add_jobs({
        [&counter](){ ++counter; },
        [&counter](){ ++counter; },
        [&counter](){ ++counter; },
        [&counter](){ ++counter; }
    });

    while(counter != 4){}

    ASSERT_EQ(counter, 4);
}

TEST(jobs, wait_for_jobs_single)
{
    iris::JobSystem js;
    std::atomic<bool> done = false;

    js.wait_for_jobs({
        [&done](){ done = true; }
    });

    ASSERT_TRUE(done);
}

TEST(jobs, wait_for_jobs_multiple)
{
    iris::JobSystem js;
    std::atomic<int> counter = 0;

    js.wait_for_jobs({
        [&counter](){ ++counter; },
        [&counter](){ ++counter; },
        [&counter](){ ++counter; },
        [&counter](){ ++counter; }
    });

    ASSERT_EQ(counter, 4);
}

TEST(jobs, wait_for_jobs_nested)
{
    iris::JobSystem js;
    std::atomic<int> counter = 0;

    js.wait_for_jobs({
        [&js, &counter]() {
            js.wait_for_jobs({
                [&js, &counter]() {
                    js.wait_for_jobs({
                        [&counter]() { ++counter; }
                    });
                    ++counter;
                }
            });
            ++counter;
        }
    });

    ASSERT_EQ(counter, 3);
}

TEST(jobs, exceptions_propogate)
{
    iris::JobSystem js;

    const auto throws = [](){ throw std::runtime_error(""); };

    ASSERT_THROW(
        js.wait_for_jobs({ throws }),
        std::runtime_error);
        
}
