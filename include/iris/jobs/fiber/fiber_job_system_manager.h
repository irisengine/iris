#pragma once

#include <memory>
#include <vector>

#include "jobs/fiber/fiber_job_system.h"
#include "jobs/job.h"
#include "jobs/job_system_manager.h"

namespace iris
{

/**
 * Implementation of JobSystemManager for FiberJobSystem.
 */
class FiberJobSystemManager : public JobSystemManager
{
  public:
    ~FiberJobSystemManager() override = default;

    /**
     * Create a JobSystem.
     *
     * @returns
     *   Pointer to JobSystem.
     */
    JobSystem *create_job_system() override;

    /**
     * Add a collection of jobs. Once added these are executed in a
     * fire-and-forget manner, there is no way to wait on them to finish or
     * to know when they have executed.
     *
     * @param jobs
     *   Jobs to execute.
     */
    void add(const std::vector<Job> &jobs) override;

    /**
     * Add a collection of jobs. Once added this call blocks until all
     * jobs have finished executing.
     *
     * @param jobs
     *   Jobs to execute.
     */
    void wait(const std::vector<Job> &jobs) override;

  private:
    /** Current JobSystem. */
    std::unique_ptr<FiberJobSystem> job_system_;
};

}
