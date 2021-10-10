#include "jobs/fiber/fiber_job_system.h"

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <exception>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "core/auto_release.h"
#include "core/semaphore.h"
#include "core/thread.h"
#include "jobs/concurrent_queue.h"
#include "jobs/fiber/counter.h"
#include "jobs/fiber/fiber.h"
#include "jobs/job.h"
#include "log/log.h"

namespace
{

/**
 * This is the main function for the worker threads. It's responsible for
 * taking fibers off the queue, executing them and performing all necessary
 * bookkeeping.
 *
 * @param id
 *   Unique id for thread.
 *
 * @param jobs_semaphore
 *   Semaphore signaling how many fibers are available to run.
 *
 * @param running
 *   Flag to indicate if this thread should keep running.
 *
 * @param fibers
 *   Queue of fibers to pop from.e
 */
void job_thread(
    int id,
    iris::Semaphore &jobs_semaphore,
    std::atomic<bool> &running,
    iris::ConcurrentQueue<std::tuple<iris::Fiber *, iris::Counter *>> &fibers)
{
    iris::Fiber::thread_to_fiber();

    LOG_DEBUG(
        "job_system",
        "{} thread start [{}]",
        id,
        (void *)*iris::Fiber::this_fiber());

    while (running)
    {
        // wait for jobs to become available
        jobs_semaphore.acquire();

        if (!running)
        {
            break;
        }

        // block and wait for fiber from queue, this should be low contention
        // as most of the waiting is handled by the semaphore
        auto [fiber, wait_counter] = fibers.dequeue();

        // we cannot safely use a fiber whilst it is resuming
        // as a fiber should never be in the resuming state for long (the time
        // it takes to suspend and return to previous context) we use a
        // primitive spin lock
        while (!fiber->is_safe())
        {
        }

        if (wait_counter == nullptr)
        {
            // if we have no wait counter then this is the first time we are
            // seeing this fibre - so start it
            fiber->start();
        }
        else
        {
            // if we have a wait counter then this is a suspended fiber thats
            // been put back on the queue

            if (*wait_counter == 0)
            {
                // wait counter of zero means all its children jobs have
                // finished so we can resume
                fiber->resume();

                // if nothing is waiting on us then we were a fire-and-forget
                // job so need to cleanup
                if (!fiber->is_being_waited_on())
                {
                    delete fiber;
                }
            }
            else
            {
                // we are still waiting on at least one child job to finish so
                // put the fiber back on the queue
                fibers.enqueue(fiber, wait_counter);
                jobs_semaphore.release();
            }
        }
    }

    LOG_DEBUG(
        "job_system",
        "{} thread end [{}]",
        id,
        (void *)*iris::Fiber::this_fiber());

    // safe to cleanup fiber we created for thread
    delete *iris::Fiber::this_fiber();
    *iris::Fiber::this_fiber() = nullptr;
}

/**
 * If the main thread (which is not a fiber) wants to wait on a job then it
 * cannot. We bootstrap that by using traditional signaling primitives.
 *
 * @param jobs
 *   Jobs to wait on
 *
 * @param js
 *   Pointer to JoSystem.
 */
void bootstrap_first_job(
    const std::vector<iris::Job> &jobs,
    iris::FiberJobSystem *js)
{
    std::mutex m;
    std::condition_variable cv;
    std::atomic<bool> done = false;
    std::exception_ptr exception;

    // wrap everything up in a fire-and-forget job
    js->add_jobs({{[&cv, &done, &jobs, &exception, js]()
                   {
                       LOG_ENGINE_INFO("job_system", "bootstrap started");

                       try
                       {
                           // we can now call wait for jobs because we are
                           // within another fiber
                           js->wait_for_jobs(jobs);
                       }
                       catch (...)
                       {
                           // capture any exception
                           exception = std::current_exception();
                       }

                       // signal calling thread we are finished
                       done = true;
                       cv.notify_one();
                       LOG_ENGINE_INFO("job_system", "bootstrap lambda done");
                   }}});

    // block and wait for wrapping fiber to finish
    if (!done)
    {
        std::unique_lock lock(m);
        cv.wait(lock, [&done]() { return done.load(); });
    }

    LOG_ENGINE_INFO("job_system", "non-fiber wait complete");

    // rethrow any exception
    if (exception)
    {
        std::rethrow_exception(exception);
    }
}

}

namespace iris
{

FiberJobSystem::FiberJobSystem()
    : running_(true)
    , jobs_semaphore_()
    , workers_()
    , fibers_()
{
    auto thread_count = std::max(1u, std::thread::hardware_concurrency() - 1u);

    LOG_ENGINE_INFO("job_system", "creating {} threads", thread_count);
    for (auto i = 0u; i < thread_count; ++i)
    {
        workers_.emplace_back(
            job_thread,
            i + 1,
            std::ref(jobs_semaphore_),
            std::ref(running_),
            std::ref(fibers_));
    }
}

FiberJobSystem::~FiberJobSystem()
{
    running_ = false;

    for (auto i = 0u; i < workers_.size() + 1u; i++)
    {
        jobs_semaphore_.release();
    }

    for (auto &worker : workers_)
    {
        worker.join();
    }
}

void FiberJobSystem::add_jobs(const std::vector<Job> &jobs)
{
    for (const auto &job : jobs)
    {
        // we rely on the worker thread to clean up after us
        auto *f = new Fiber{job};

        fibers_.enqueue(f, nullptr);
        jobs_semaphore_.release();
    }
}

void FiberJobSystem::wait_for_jobs(const std::vector<Job> &jobs)
{
    if (*Fiber::this_fiber() == nullptr)
    {
        bootstrap_first_job(jobs, this);
    }
    else
    {
        auto counter = std::make_unique<Counter>(static_cast<int>(jobs.size()));
        std::vector<std::unique_ptr<Fiber>> fibers{};

        // create fibers and add to the queue
        for (const auto &job : jobs)
        {
            fibers.emplace_back(std::make_unique<Fiber>(job, counter.get()));

            fibers_.enqueue(fibers.back().get(), nullptr);
            jobs_semaphore_.release();
        }

        // mark current fiber as unsafe (so another thread doesn't preemptively
        // try to resume it), stick it on the queue
        (*Fiber::this_fiber())->set_unsafe();
        fibers_.enqueue(*Fiber::this_fiber(), counter.get());
        jobs_semaphore_.release();

        // suspend current thread - this will internally mark the fiber as safe
        (*Fiber::this_fiber())->suspend();

        // the above line will not return
        // if we get there then all children jobs have finished and resume has
        // been called

        std::exception_ptr job_exception;

        for (const auto &fiber : fibers)
        {
            // find first exception that was throw, first come first served
            if ((fiber->exception() != nullptr) && !job_exception)
            {
                job_exception = fiber->exception();
                break;
            }
        }

        if (job_exception)
        {
            std::rethrow_exception(job_exception);
        }
    }
}

}
