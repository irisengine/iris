#include "jobs/job_system.h"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

#include "jobs/concurrent_queue.h"
#include "jobs/fiber/counter.h"
#include "jobs/fiber/fiber.h"
#include "jobs/fiber/fiber_pool.h"
#include "jobs/job.h"
#include "log/log.h"
#include "platform/thread.h"

namespace
{

/**
 * Struct for tracking stats about the job system.
 */
struct Stats
{
    Stats()
        : id(-1),
          jobs_started(0),
          jobs_ended(0),
          wait_time(0),
          run_time(0)
    { }

    int id;
    std::uint32_t jobs_started;
    std::uint32_t jobs_ended;
    std::chrono::duration<int, std::milli> wait_time;
    std::chrono::duration<int, std::milli> run_time;
};

/**
 * This is the main function for the worker threads. It's responsible for
 * taking fibers off the queue, executing them and performing all necessary
 * bookkeeping.
 *
 * @param id
 *   Unique id for thread.
 *
 * @param fibers
 *   Queue of fibers to pop from.
 *
 * @param signal_mutex
 *   Mutex for signalling when new fibers are available.
 *
 * @param signal_condition
 *   Condition variable for signalling when new fibers are available.
 *
 * @param running
 *   Flag to indicate if this thread should keep running.
 *
 * @param stats
 *   Pointer to a struct to populate with stats.
 *
 * @param fiber_pool
 *   A pool from which to allocate/release new fibers.
 */
void job_thread(
    int id,
    iris::ConcurrentQueue<std::tuple<iris::Fiber*, iris::Counter*>> &fibers,
    std::mutex &signal_mutex,
    std::condition_variable &signal_condition,
    std::atomic<bool> &running,
    Stats *stats,
    iris::DefaultFiberPool &fiber_pool)
{
    LOG_ENGINE_INFO("job_system", "starting thread: {}", id);

    const auto start = std::chrono::high_resolution_clock::now();

    while(running)
    {
        iris::Fiber *f = nullptr;

        const auto wait_start = std::chrono::high_resolution_clock::now();

        // if there are no fibers then wait wait to be signaled
        // we don't wait on the queue itself as we want it to have minimal
        // contention
        if(fibers.empty())
        {
            std::unique_lock lock(signal_mutex);
            signal_condition.wait(lock, [&fibers, &running](){
                return !running || !fibers.empty();
            });
        }

        if(!running)
        {
            break;
        }

        std::tuple<iris::Fiber*, iris::Counter*> element;

        // try and get next fiber from queue
        // this may not be the thread that started the fiber!
        if(fibers.try_dequeue(element))
        {
            f = std::get<0>(element);
            auto *counter = std::get<1>(element);

            // if we have a counter then this is a fiber that is waiting on
            // other fibers to complete
            if(counter != nullptr)
            {
                if(f->state() == iris::FiberState::RESUMABLE)
                {
                    // if a fiber is resumable then we need to check if its
                    // counter has finished, if it hasn't then put it back on
                    // the queue
                    if(*counter != 0)
                    {
                        fibers.enqueue(f, counter);
                        f = nullptr;
                    }
                }
                else if(f->state() == iris::FiberState::PAUSING)
                {
                    // a fiber is not instantly read to be resumed after it has
                    // been suspended (it has a certain amount of internal
                    // work to do)
                    // if we are in the special pausing state the only safe
                    // thing we can do is put it back on the queue to try again
                    // later
                    fibers.enqueue(f, counter);
                    f = nullptr;
                }
            }
        }

        // calculate time spent waiting for a fiber and store it in stats
        const auto wait_end = std::chrono::high_resolution_clock::now();
        stats->wait_time += std::chrono::duration_cast<std::chrono::milliseconds>(wait_end - wait_start);

        LOG_ENGINE_INFO("job_system", "{} waking up with {}", id, f);

        // signal other worker threads to try and get a fiber
        signal_condition.notify_all();

        if(f != nullptr)
        {
            if(f->state() == iris::FiberState::RESUMABLE)
            {
                // a paused fiber should be resumed
                f->resume();

                fiber_pool.release(f);
                stats->jobs_ended++;
            }
            else
            {
                stats->jobs_started++;

                // a new fiber should be started
                f->start();

                // if after the fiber has run we are not paused then safe
                // to release
                if(f->state() == iris::FiberState::READY)
                {
                    fiber_pool.release(f);

                    stats->jobs_ended++;
                }
            }
        }
    }

    const auto end = std::chrono::high_resolution_clock::now();

    stats->run_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

/**
 * Calculate stats about the job system execution.
 *
 * @param stream
 *   Stream to write stats to.
 *
 * @param stats
 *   Collection of thread stats.
 *
 * @param fiber_pool
 *   The fiber pool used during execution.
 */
void calculate_stats(
    std::ostream &stream,
    const std::vector<Stats> &stats,
    iris::DefaultFiberPool &fiber_pool)
{
    auto total_jobs_started = 0u;
    auto total_jobs_ended = 0u;
    auto average_wait_time = 0u;
    auto average_run_time = 0u;
    auto average_blocked_percentage = 0.0f;

    for(const auto &stat : stats)
    {
        auto percent_blocked = 100.0f *
            (static_cast<float>(stat.wait_time.count()) /static_cast<float>(stat.run_time.count()));

        stream << "thread: " << stat.id << std::endl;
        stream << "\t" << "jobs started: " << stat.jobs_started << std::endl;
        stream << "\t" << "jobs ended: " << stat.jobs_ended << std::endl;
        stream << "\t" << "wait time: " << stat.wait_time.count() << "ms" << std::endl;
        stream << "\t" << "run time: " << stat.run_time.count() << "ms" << std::endl;
        stream << "\t" << "percent blocked: " << percent_blocked << "%" << std::endl;

        total_jobs_started += stat.jobs_started;
        total_jobs_ended += stat.jobs_ended;
        average_wait_time += stat.wait_time.count();
        average_run_time += stat.run_time.count();
        average_blocked_percentage += percent_blocked;
    }

    stream << "======================" << std::endl;
    stream << "total jobs started: " << total_jobs_started << std::endl;
    stream << "total jobs ended: " << total_jobs_ended << std::endl;
    stream << "average wait time: " << average_wait_time / static_cast<float>(stats.size()) << "ms" << std::endl;
    stream << "average run time: " << average_run_time / static_cast<float>(stats.size()) << "ms" << std::endl;
    stream << "average percent blocked: " << average_blocked_percentage / static_cast<float>(stats.size()) << "%" << std::endl;
    stream << "allocated " << fiber_pool.capacity() << " fibers" << std::endl;
}

}

namespace iris
{

struct JobSystem::implementation
{
    ConcurrentQueue<std::tuple<Fiber*, Counter*>> fibers;
    std::mutex signal_mutex;
    std::condition_variable signal_condition;
    std::vector<Thread> workers;
    std::vector<Stats> stats;
    DefaultFiberPool fiber_pool;
};

JobSystem::JobSystem()
    : impl_(std::make_unique<implementation>()),
      running_(true),
      stats_stream_(nullptr)
{
    auto thread_count = std::thread::hardware_concurrency();

    impl_->stats.resize(thread_count);

    LOG_ENGINE_INFO("job_system", "creating {} threads", thread_count);
    for(auto i = 0u; i < thread_count; ++i)
    {
        impl_->stats[i].id = i + 1;
        impl_->workers.emplace_back(
            job_thread,
            i + 1,
            std::ref(impl_->fibers),
            std::ref(impl_->signal_mutex),
            std::ref(impl_->signal_condition),
            std::ref(running_),
            std::addressof(impl_->stats[i]),
            std::ref(impl_->fiber_pool));
    }
}

JobSystem::~JobSystem()
{
    running_ = false;

    // signal all threads to wakeup and stop
    for(auto i = 0u; i < impl_->workers.size() + 1u; i++)
    {
        impl_->signal_condition.notify_all();
    }

    for(auto &worker : impl_->workers)
    {
        worker.join();
    }

    if(stats_stream_ != nullptr)
    {
        calculate_stats(*stats_stream_, impl_->stats, impl_->fiber_pool);
    }
}

void JobSystem::add_jobs(const std::vector<Job> &jobs)
{
    for(const auto &job : jobs)
    {
        // create new fiber for job
        auto *f = impl_->fiber_pool.next();
        f->reset(job, nullptr);

        impl_->fibers.enqueue(f, nullptr);

        // signal workers that a new fiber is available
        impl_->signal_condition.notify_all();
    }
}

void JobSystem::wait_for_jobs(const std::vector<Job> &jobs)
{
    if(*Fiber::this_fiber() == nullptr)
    {
        // special case when there is no current fiber i.e. this is being
        // called from the main thread
        bootstrap_first_job(jobs);
    }
    else
    {
        // create new atomic counter
        auto counter = std::make_unique<Counter>(static_cast<int>(jobs.size()));

        // put fiber in pausing state, this will prevent it from being
        // prematurely resumed
        (*Fiber::this_fiber())->set_state(FiberState::PAUSING);

        for(auto &j : jobs)
        {
            // create new fiber for job
            auto *f = impl_->fiber_pool.next();
            f->reset(j, counter.get());

            // add fiber to queue
            impl_->fibers.enqueue(f, nullptr);
        }

        // add this fiber back to the queue with the new counter
        impl_->fibers.enqueue(*Fiber::this_fiber(), counter.get());

        impl_->signal_condition.notify_all();

        // suspend current fiber
        (*Fiber::this_fiber())->suspend();
    }
}

void JobSystem::set_stats_stream(std::ostream *stats_stream)
{
    stats_stream_ = stats_stream;
}

/**
 * If a fiber is started from the main thread (i.e. not from another fiber)
 * then we run into problems when trying to wait for it as we have nothing to
 * suspend. This function wraps up the wait job in a fire-and-forget job but
 * then manually blocks the calling thread until it has finished.
 *
 * @param jobs
 *   Jobs to execute.
 */
void JobSystem::bootstrap_first_job(const std::vector<Job> &jobs)
{
    std::mutex m;
    std::condition_variable cv;
    std::atomic<bool> done = false;
    std::exception_ptr exception;

    // wrap everything up in a fire-and-forget job
    add_jobs({{
        [&cv, &done, &jobs, &exception, this]()
        {
            LOG_ENGINE_INFO("job_system", "bootstrap started");

            try
            {
                // we can now call wait for jobs because we are within another
                // fiber
                wait_for_jobs(jobs);
            }
            catch(...)
            {
                // capture any exception
                exception = std::current_exception();
            }

            // signal calling thread we are finished
            done = true;
            cv.notify_one();
            LOG_ENGINE_INFO("job_system", "bootstrap lambda done");
        }
    }});

    // block and wait for wrapping fiber to finish
    if(!done)
    {
        std::unique_lock lock(m);
        cv.wait(lock, [&done]() { return done.load(); });
    }

    LOG_ENGINE_INFO("job_system", "non-fiber wait complete");

    // rethrow any exception
    if(exception)
    {
        std::rethrow_exception(exception);
    }
}

}

