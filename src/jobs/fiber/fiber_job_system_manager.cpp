#include "jobs/fiber/fiber_job_system_manager.h"

#include <memory>
#include <vector>

#include "core/error_handling.h"
#include "jobs/fiber/fiber_job_system.h"
#include "jobs/job.h"
#include "jobs/job_system_manager.h"

namespace iris
{

JobSystem *FiberJobSystemManager::create_job_system()
{
    ensure(!job_system_, "job system already created");

    job_system_ = std::make_unique<FiberJobSystem>();
    return job_system_.get();
}

void FiberJobSystemManager::add(const std::vector<Job> &jobs)
{
    job_system_->add_jobs(jobs);
}

void FiberJobSystemManager::wait(const std::vector<Job> &jobs)
{
    job_system_->wait_for_jobs(jobs);
}
}
