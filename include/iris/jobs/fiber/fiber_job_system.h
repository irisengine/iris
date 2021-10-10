////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atomic>
#include <memory>
#include <vector>

#include "core/semaphore.h"
#include "core/thread.h"
#include "jobs/concurrent_queue.h"
#include "jobs/fiber/counter.h"
#include "jobs/fiber/fiber.h"
#include "jobs/job.h"
#include "jobs/job_system.h"

namespace iris
{

/**
 * Implementation of JobSystem that schedules its jobs using fibers.
 */
class FiberJobSystem : public JobSystem
{
  public:
    FiberJobSystem();
    ~FiberJobSystem() override;

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

    /** Semaphore signally how many fibers are available. */
    Semaphore jobs_semaphore_;

    /** Worker threads which execute fibers. */
    std::vector<Thread> workers_;

    /** Queue of fibers.*/
    ConcurrentQueue<std::tuple<Fiber *, Counter *>> fibers_;
};

}
