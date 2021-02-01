#include "jobs/job_system.h"

#include <chrono>
#include <future>
#include <vector>

#include "jobs/job.h"
#include "log/log.h"

namespace iris
{

struct JobSystem::implementation
{
};

JobSystem::JobSystem()
    : running_(true)
    , impl_(nullptr)
{
}

JobSystem::~JobSystem() = default;

void JobSystem::add_jobs(const std::vector<Job> &jobs)
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

void JobSystem::wait_for_jobs(const std::vector<Job> &jobs)
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
