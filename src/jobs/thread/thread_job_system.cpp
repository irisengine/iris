////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "jobs/thread/thread_job_system.h"

#include <chrono>
#include <future>
#include <vector>

#include "jobs/job.h"
#include "log/log.h"

namespace iris
{

ThreadJobSystem::ThreadJobSystem()
    : running_(true)
{
}

void ThreadJobSystem::add_jobs(const std::vector<Job> &jobs)
{
    for (const auto &job : jobs)
    {
        // we cannot simply call std::async here as the std::future destructor
        // will block and wait for the job to finish
        // instead we create a shared_ptr for the future and copy it by value
        // into the async task
        // this ensures it keeps a reference to itself and won't go out of
        // scope until the job is complete
        auto future = std::make_shared<std::future<void>>();

        *future = std::async(std::launch::async, [future, job] { job(); });
    }
}

void ThreadJobSystem::wait_for_jobs(const std::vector<Job> &jobs)
{
    std::vector<std::future<void>> waiting_jobs{};

    for (const auto &job : jobs)
    {
        waiting_jobs.emplace_back(std::async(std::launch::async, job));
    }

    for (auto &waiting_job : waiting_jobs)
    {
        waiting_job.get();
    }
}
}
