#pragma once

#include <atomic>
#include <memory>

#include "jobs/fiber/counter.h"
#include "jobs/fiber/fiber_state.h"
#include "jobs/job.h"

namespace eng
{

// forward declaration, will be arch specific
struct Context;

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
         * Construct an empty Fiber which does not represent a job. In this
         * state the only defined behaviour is to call reset.
         */
        Fiber();

        /**
         * Construct a Fiber with a job to run.
         */
        explicit Fiber(const Job &job);

        /**
         * Free all resources allocated for the Fiber.
         */
        ~Fiber();

        // disable copying
        Fiber(const Fiber&) = delete;
        Fiber& operator=(const Fiber&) = delete;

        /**
         * Move constructor. 
         *
         * @param other
         *   Fiber to move from.
         */
        Fiber(Fiber &&other);

        /**
         * Move assignment.
         *
         * @param other
         *   Fiber to move from.
         *
         * @returns
         *   Reference to this Fiber after move.
         */
        Fiber& operator=(Fiber &&other);

        /**
         * Reset this fiber to represent a new job, with an optional counter.
         *
         * This will reuse the existing stack, for efficiency it is not zeroed.
         *
         * It is undefined behaviour to reset a Fiber if it has already started
         * but not finished.
         *
         * @param job
         *   New job to run.
         *
         * @param counter
         *   Optional counter. If not nullptr will be decremented when fiber
         *   is reset.
         */
        void reset(const Job &job, Counter *counter);

        /**
         * Start the fiber.
         */
        void start();

        /**
         * To be called when a fiber has completed, but is not going to be
         * destructed.
         *
         * It is undefined behaviour to call this when a fiber is still running.
         */
        void finish();

        /**
         * Suspends a Fibers execution, execution will continue from where
         * start was called.
         */
        void suspend();

        /**
         * Resume a suspended Fiber. Execution will continue from where suspend
         * was called. If the running job threw an uncaught exception it is
         * rethrown here.
         *
         * It is undefined behaviour to resume a non-suspended Fiber.
         */
        void resume();

        /**
         * Gets a pointer to the current fiber running on the calling thread.
         * If no Fiber is running then nullptr.
         *
         * @returns
         *   Pointer to pointer to running Fiber, or nullptr if no Fiber is
         *   running.
         */
        static Fiber** this_fiber();

        /**
         * Get counter, maybe nullptr.
         *
         * @returns
         *   Fiber counter.
         */
        Counter* counter();

        /**
         * Get the state of the Fiber.
         *
         * @returns
         *   Fiber state.
         */
        FiberState state();

        /**
         * Set state of Fiber.
         *
         * @param state
         *   New Fiber state.
         */
        void set_state(FiberState state);

    private:

        // helper methods, see cpp file for documentation
        // no-inline as the implementation relies on calling these as functions

        __attribute__((noinline)) void do_start();

        __attribute__((noinline)) void do_suspend();

        __attribute__((noinline)) int do_resume();

        /** Pointer to top of stack. */
        void *stack_;

        /** Pointer to start of allocated stack space. */
        void *stack_origin_;
        
        /** Job to run in Fiber. */
        Job job_;

        /** Saved context from start(). */
        Context *context_;

        /** Saved context from suspend(). */
        Context *suspended_context_;

        /** optional counter. */
        Counter *counter_;

        /** Optional parent fiber. */
        Fiber *parent_fiber_;

        /** Pointer storing job exception. */
        std::exception_ptr exception_;

        /** Flag indicating we have an exception. */
        std::atomic<bool> has_exception_;

        /** State of Fiber. */
        std::atomic<FiberState> state_;
};

}

