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

        /**
         * Create a new job system.
         */
        JobSystem();

        // Disable copy/move
        ~JobSystem();
        JobSystem(const JobSystem&) = delete;
        JobSystem& operator=(const JobSystem&) = delete;
        JobSystem(JobSystem&&) = delete;
        JobSystem& operator=(JobSystem&&) = delete;

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

        /**
         * Set a stream the job system should write statistics to. If set stats
         * will be written on destruction.
         *
         * What stats are written are dependant on the implementation.
         *
         * @param stats_stream
         *   Pointer to stream to write to.
         */
        void set_stats_stream(std::ostream *stats_stream);

    private:

        // helper method, see cpp file for documentation
        void bootstrap_first_job(const std::vector<Job> &jobs);

        /** Pointer to implementation. */
        struct implementation;
        std::unique_ptr<implementation> impl_;

        /** Flag indicating of system is running. */
        std::atomic<bool> running_;

        /** Optional stream to write stats to. */
        std::ostream *stats_stream_;
};

}

