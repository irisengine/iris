////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#include "jobs/job.h"

namespace iris
{

/**
 * Interface for a class managing the scheduling and running of jobs.
 */
class JobSystem
{
  public:
    JobSystem() = default;
    virtual ~JobSystem() = default;

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
    virtual void add_jobs(const std::vector<Job> &jobs) = 0;

    /**
     * Add a collection of jobs. Once added this call blocks until all
     * jobs have finished executing.
     *
     * @param jobs
     *   Jobs to execute.
     */
    virtual void wait_for_jobs(const std::vector<Job> &jobs) = 0;
};

}
