#pragma once

#include <atomic>
#include <memory>
#include <ostream>
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
    static JobSystem &instance()
    {
        static JobSystem job_system;
        return job_system;
    }

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
    static void add_jobs(const std::vector<Job> &jobs)
    {
        auto &js = JobSystem::instance();
        js.add_jobs_impl(jobs);
    }

    /**
     * Add a collection of jobs. Once added this call blocks until all
     * jobs have finished executing.
     *
     * @param jobs
     *   Jobs to execute.
     */
    static void wait_for_jobs(const std::vector<Job> &jobs)
    {
        auto &js = JobSystem::instance();
        js.wait_for_jobs_impl(jobs);
    }

    /**
     * Set a stream the job system should write statistics to. If set stats
     * will be written on destruction.
     *
     * What stats are written are dependent on the implementation.
     *
     * @param stats_stream
     *   Pointer to stream to write to.
     */
    static void set_stats_stream(std::ostream *stats_stream)
    {
        auto &js = JobSystem::instance();
        js.set_stats_stream_impl(stats_stream);
    }

  private:
    /**
     * Create a new job system.
     */
    JobSystem();

    // helper method, see cpp file for documentation
    void bootstrap_first_job(const std::vector<Job> &jobs);

    // implementation for static functions, see above for documentation

    void add_jobs_impl(const std::vector<Job> &jobs);

    void wait_for_jobs_impl(const std::vector<Job> &jobs);

    void set_stats_stream_impl(std::ostream *stats_stream);

    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;

    /** Flag indicating of system is running. */
    std::atomic<bool> running_;

    /** Optional stream to write stats to. */
    std::ostream *stats_stream_;
};

}
