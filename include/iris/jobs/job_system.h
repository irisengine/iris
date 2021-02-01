#pragma once

#include <atomic>
#include <memory>
#include <vector>

#include "jobs/job.h"

namespace iris
{

/**
 * Class for managing the scheduling and running of jobs.
 */
class JobSystem
{
  public:
    /**
     * Create a new job system.
     */
    JobSystem();

    ~JobSystem();
    JobSystem(const JobSystem &) = delete;
    JobSystem &operator=(const JobSystem &) = delete;
    JobSystem(JobSystem &&) = delete;
    JobSystem &operator=(JobSystem &&) = delete;

    /**
     * Add a collection of jobs. Once added these are executed in a
     * fire-and-forget manner, there is no way to wait on them to finish or
     * to know when they have executed.
     *
     * @param jobs
     *   Jobs to execute.
     */
    void add_jobs(const std::vector<Job> &jobs);

    /**
     * Add a collection of jobs. Once added this call blocks until all
     * jobs have finished executing.
     *
     * @param jobs
     *   Jobs to execute.
     */
    void wait_for_jobs(const std::vector<Job> &jobs);

  private:
    /** Flag indicating of system is running. */
    std::atomic<bool> running_;

    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
