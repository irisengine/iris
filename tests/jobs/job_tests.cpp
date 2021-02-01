#include <atomic>
#include <stdexcept>
#include <thread>

#include <jobs/job_system.h>

#include <gtest/gtest.h>

TEST(jobs, add_jobs_single)
{
    iris::JobSystem js;
    std::atomic<int> counter = 0;

    js.add_jobs({[&counter]() { ++counter; }});

    while (counter != 1)
    {
    }

    ASSERT_EQ(counter, 1);
}

TEST(jobs, add_jobs_multiple)
{
    iris::JobSystem js;
    std::atomic<int> counter = 0;

    js.add_jobs(
        {[&counter]() { ++counter; },
         [&counter]() { ++counter; },
         [&counter]() { ++counter; },
         [&counter]() { ++counter; }});

    while (counter != 4)
    {
    }

    ASSERT_EQ(counter, 4);
}

TEST(jobs, wait_for_jobs_single)
{
    iris::JobSystem js;
    std::atomic<bool> done = false;

    js.wait_for_jobs({[&done]() { done = true; }});

    ASSERT_TRUE(done);
}

TEST(jobs, wait_for_jobs_multiple)
{
    iris::JobSystem js;
    std::atomic<int> counter = 0;

    js.wait_for_jobs(
        {[&counter]() {
             std::this_thread::sleep_for(std::chrono::milliseconds(1000));
             ++counter;
         },
         [&counter]() { ++counter; },
         [&counter]() { ++counter; },
         [&counter]() { ++counter; }});

    ASSERT_EQ(counter, 4);
}

TEST(jobs, wait_for_jobs_nested)
{
    iris::JobSystem js;
    std::atomic<int> counter = 0;

    js.wait_for_jobs({[&counter, &js]() {
        js.wait_for_jobs({[&counter, &js]() {
            js.wait_for_jobs({[&counter, &js]() { ++counter; }});
            ++counter;
        }});
        ++counter;
    }});

    ASSERT_EQ(counter, 3);
}

TEST(jobs, wait_for_jobs_sequential)
{
    iris::JobSystem js;
    std::atomic<int> counter = 0;

    js.wait_for_jobs({[&counter]() { ++counter; }});
    js.wait_for_jobs({[&counter]() { ++counter; }});

    ASSERT_EQ(counter, 2);
}

TEST(jobs, exceptions_propagate)
{
    iris::JobSystem js{};
    const auto throws = []() { throw std::runtime_error(""); };
    ASSERT_THROW(js.wait_for_jobs({throws}), std::runtime_error);
}

TEST(jobs, exceptions_propagate_complex)
{
    iris::JobSystem js{};
    ASSERT_THROW(
        js.wait_for_jobs(
            {[&js]() {
                 js.wait_for_jobs(
                     {[]() {},
                      [&js]() {
                          js.wait_for_jobs(
                              {[&js]() { throw std::runtime_error(""); }});
                      },
                      []() {},
                      []() {}});
             },
             []() {}}),
        std::runtime_error);
}

TEST(jobs, exceptions_propagate_first_job)
{
    iris::JobSystem js{};
    ASSERT_THROW(
        js.wait_for_jobs({[&js]() { throw std::runtime_error(""); }}),
        std::runtime_error);
}
