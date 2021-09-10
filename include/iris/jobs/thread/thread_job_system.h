#pragma once

#include <atomic>
#include <memory>
#include <vector>

#include "jobs/job.h"
#include "jobs/job_system.h"

namespace iris
{

/**
 * Implementation of JobSystem that schedules its jobs using threads.
 */
class ThreadJobSystem : public JobSystem
{
  public:
    ThreadJobSystem();
    ~ThreadJobSystem() override = default;

    /**
     * Add a collection of jobs. Once added these are executed in a
     * fire-and-forget manner, there is no way to wait on them to finish or
     * to know when they have executed.
     *
     * @param jobs
     *   Jobs to execute.
     */
    void add_jobs(const std::vector<Job> &jobs) override;

    /**
     * Add a collection of jobs. Once added this call blocks until all
     * jobs have finished executing.
     *
     * @param jobs
     *   Jobs to execute.
     */
    void wait_for_jobs(const std::vector<Job> &jobs) override;

  private:
    /** Flag indicating of system is running. */
    std::atomic<bool> running_;
};

}
