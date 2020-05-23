#include "jobs/job_system.h"

#include <future>
#include <vector>

#include "jobs/job.h"

namespace eng
{

struct JobSystem::implementation
{
    std::size_t async_count;
};

JobSystem::JobSystem()
    : impl_(std::make_unique<implementation>()),
      running_(true),
      stats_stream_(nullptr)
{
    impl_->async_count = 0;
}

JobSystem::~JobSystem()
{
    if(stats_stream_ != nullptr)
    {
        *stats_stream_ << "created " << impl_->async_count << " tasks" << std::endl;
    }
}

void JobSystem::add_jobs(const std::vector<Job> &jobs)
{
    for(const auto &job : jobs)
    {
        std::async(std::launch::async, job);
        impl_->async_count++;
    }
}

void JobSystem::wait_for_jobs(const std::vector<Job> &jobs)
{
    std::vector<std::future<void>> waiting_jobs{ };

    for(const auto &job : jobs)
    {
        waiting_jobs.emplace_back(std::async(std::launch::async, job));
        impl_->async_count++;
    }

    for(auto &waiting_job : waiting_jobs)
    {
        waiting_job.get();
    }
}

void JobSystem::set_stats_stream(std::ostream *stats_stream)
{
    stats_stream_ = stats_stream;
}

}

