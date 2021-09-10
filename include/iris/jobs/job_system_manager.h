#pragma once

#include <vector>

#include "jobs/job.h"
#include "jobs/job_system.h"

namespace iris
{

/**
 * Interface for a class that manages JobSystem objects. This used as part of
 * component registration in the Root.
 */
class JobSystemManager
{
  public:
    virtual ~JobSystemManager() = default;

    /**
     * Create a JobSystem.
     *
     * @returns
     *   Pointer to JobSystem.
     */
    virtual JobSystem *create_job_system() = 0;

    /**
     * Add a collection of jobs. Once added these are executed in a
     * fire-and-forget manner, there is no way to wait on them to finish or
     * to know when they have executed.
     *
     * @param jobs
     *   Jobs to execute.
     */
    virtual void add(const std::vector<Job> &jobs) = 0;

    /**
     * Add a collection of jobs. Once added this call blocks until all
     * jobs have finished executing.
     *
     * @param jobs
     *   Jobs to execute.
     */
    virtual void wait(const std::vector<Job> &jobs) = 0;
};

}
