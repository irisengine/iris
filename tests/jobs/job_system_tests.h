////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <atomic>
#include <stdexcept>
#include <thread>

#include <jobs/job_system.h>

#include <gtest/gtest.h>

template <class T>
class JobSystemTests : public ::testing::Test
{
  protected:
    T js_;
};

TYPED_TEST_SUITE_P(JobSystemTests);

TYPED_TEST_P(JobSystemTests, add_jobs_single)
{
    std::atomic<int> counter = 0;

    this->js_.add_jobs({[&counter]() { ++counter; }});

    while (counter != 1)
    {
    }

    ASSERT_EQ(counter, 1);
}

TYPED_TEST_P(JobSystemTests, add_jobs_multiple)
{
    std::atomic<int> counter = 0;

    this->js_.add_jobs(
        {[&counter]() { ++counter; },
         [&counter]() { ++counter; },
         [&counter]() { ++counter; },
         [&counter]() { ++counter; }});

    while (counter != 4)
    {
    }

    ASSERT_EQ(counter, 4);
}

TYPED_TEST_P(JobSystemTests, wait_for_jobs_single)
{
    std::atomic<bool> done = false;

    this->js_.wait_for_jobs({[&done]() { done = true; }});

    ASSERT_TRUE(done);
}

TYPED_TEST_P(JobSystemTests, wait_for_jobs_multiple)
{
    std::atomic<int> counter = 0;

    this->js_.wait_for_jobs(
        {[&counter]() {
             std::this_thread::sleep_for(std::chrono::milliseconds(1000));
             ++counter;
         },
         [&counter]() { ++counter; },
         [&counter]() { ++counter; },
         [&counter]() { ++counter; }});

    ASSERT_EQ(counter, 4);
}

TYPED_TEST_P(JobSystemTests, wait_for_jobs_nested)
{
    std::atomic<int> counter = 0;

    this->js_.wait_for_jobs({[&counter, this]() {
        this->js_.wait_for_jobs({[&counter, this]() {
            this->js_.wait_for_jobs({[&counter]() { ++counter; }});
            ++counter;
        }});
        ++counter;
    }});

    ASSERT_EQ(counter, 3);
}

TYPED_TEST_P(JobSystemTests, wait_for_jobs_sequential)
{
    std::atomic<int> counter = 0;

    this->js_.wait_for_jobs({[&counter]() { ++counter; }});
    this->js_.wait_for_jobs({[&counter]() { ++counter; }});

    ASSERT_EQ(counter, 2);
}

TYPED_TEST_P(JobSystemTests, exceptions_propagate)
{
    const auto throws = []() { throw std::runtime_error(""); };
    ASSERT_THROW(this->js_.wait_for_jobs({throws}), std::runtime_error);
}

TYPED_TEST_P(JobSystemTests, exceptions_propagate_complex)
{
    ASSERT_THROW(
        this->js_.wait_for_jobs(
            {[this]() {
                 this->js_.wait_for_jobs(
                     {[]() {},
                      [this]() {
                          this->js_.wait_for_jobs(
                              {[]() { throw std::runtime_error(""); }});
                      },
                      []() {},
                      []() {}});
             },
             []() {}}),
        std::runtime_error);
}

TYPED_TEST_P(JobSystemTests, exceptions_propagate_first_job)
{
    ASSERT_THROW(
        this->js_.wait_for_jobs({[]() { throw std::runtime_error(""); }}),
        std::runtime_error);
}

REGISTER_TYPED_TEST_SUITE_P(
    JobSystemTests,
    add_jobs_single,
    add_jobs_multiple,
    wait_for_jobs_single,
    wait_for_jobs_multiple,
    wait_for_jobs_nested,
    wait_for_jobs_sequential,
    exceptions_propagate,
    exceptions_propagate_complex,
    exceptions_propagate_first_job);
