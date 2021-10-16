////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atomic>
#include <cstddef>
#include <memory>

#include "core/static_buffer.h"
#include "jobs/fiber/counter.h"
#include "jobs/job.h"

namespace iris
{

/*
 * A Fiber is a user-land thread. It maintains it's own stack and can be
 * suspended and resumed (cooperative multi-threading). A Fiber will be started
 * from a thread but can be suspended and resumed from a different one. In
 * general one should not have to manually create Fibers, they are an internal
 * class. For parallelising work the JobSystem should be used.
 */
class Fiber
{
  public:
    /**
     * Construct a Fiber with a job to run.
     *
     * @param job
     *   Job to run.
     */
    explicit Fiber(Job job);

    /**
     * Construct a Fiber with a job and a counter.
     *
     * @param job
     *   Job to run.
     *
     * @param counter
     *   Counter to decrement when job is done.
     */
    Fiber(Job job, Counter *counter);

    ~Fiber();

    Fiber(const Fiber &) = delete;
    Fiber &operator=(const Fiber &) = delete;
    Fiber(Fiber &&other) = delete;
    Fiber &operator=(Fiber &&other) = delete;

    /**
     * Start the fiber.
     */
    void start();

    /**
     * Suspends a Fibers execution, execution will continue from where
     * start was called.
     */
    void suspend();

    /**
     * Resume a suspended Fiber. Execution will continue from where suspend
     * was called.
     *
     * It is undefined behavior to resume a non-suspended Fiber.
     */
    void resume();

    /**
     * Check if a Fiber is safe to call methods on. It is only not safe when it
     * has been suspended but not yet restored its original context.
     *
     * @returns
     *   True if this Fiber is safe false otherwise.
     */
    bool is_safe() const;

    /**
     * Set fiber to be unsafe.
     */
    void set_unsafe();

    /**
     * Check if another fiber is waiting for this to finish.
     *
     * @returns
     *   True if another fiber is waiting on this, otherwise false.
     */
    bool is_being_waited_on() const;

    /**
     * Get any exception thrown during the execution of this fiber.
     *
     * @returns
     *   exception_ptr to throw exception, nullptr of none were thrown.
     */
    std::exception_ptr exception() const;

    /**
     * Convert current thread to fiber. This must be called once (and only once)
     * on each thread that wants to execute fibers.
     */
    static void thread_to_fiber();

    /**
     * Gets a pointer to the current fiber running on the calling thread.
     * If no Fiber is running then nullptr.
     *
     * @returns
     *   Pointer to pointer to running Fiber, or nullptr if no Fiber is
     *   running.
     */
    static Fiber **this_fiber();

  private:
    /** Job to run in Fiber. */
    Job job_;

    /** optional counter. */
    Counter *counter_;

    /** Optional parent fiber. */
    Fiber *parent_fiber_;

    /** Pointer storing job exception. */
    std::exception_ptr exception_;

    /** Flag if fiber is not safe to operator on. */
    std::atomic<bool> safe_;

    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
