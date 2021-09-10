#include "jobs/thread/thread_job_system_manager.h"

#include <memory>
#include <vector>

#include "core/exception.h"
#include "jobs/job.h"
#include "jobs/job_system_manager.h"
#include "jobs/thread/thread_job_system.h"

namespace iris
{

JobSystem *ThreadJobSystemManager::create_job_system()
{
    if (job_system_)
    {
        throw Exception("job system already created");
    }

    job_system_ = std::make_unique<ThreadJobSystem>();
    return job_system_.get();
}

void ThreadJobSystemManager::add(const std::vector<Job> &jobs)
{
    job_system_->add_jobs(jobs);
}

void ThreadJobSystemManager::wait(const std::vector<Job> &jobs)
{
    job_system_->wait_for_jobs(jobs);
}

}
