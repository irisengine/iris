#include "jobs/fiber/fiber.h"

#include <atomic>
#include <cstddef>
#include <exception>

#include <sys/mman.h>

#include "core/exception.h"
#include "jobs/context.h"
#include "jobs/fiber/fiber_state.h"
#include "jobs/job.h"
#include "log/log.h"
#include "platform/static_buffer.h"

extern "C"
{

    // these will be defined with arch specific assembler
    extern void get_context(iris::Context *);
    extern void set_context(iris::Context *);
    extern void change_stack(void *stack);
}

namespace iris
{

Fiber::Fiber()
    : Fiber(Job{})
{
}

Fiber::Fiber(const Job &job)
    : stack_buffer_(10)
    , stack_(nullptr)
    , job_(job)
    , context_(nullptr)
    , suspended_context_(nullptr)
    , counter_(nullptr)
    , parent_fiber_(nullptr)
    , exception_(nullptr)
    , has_exception_(false)
    , state_(FiberState::READY)
{
    reset(job, nullptr);
}

void Fiber::reset(const Job &job, Counter *counter)
{
    job_ = job;
    state_ = FiberState::READY;
    parent_fiber_ = *this_fiber();
    counter_ = counter;

    // stack grows from high -> low memory so move our pointer down, not all the
    // way as we need some space to copy the previous stack frame
    stack_ = stack_buffer_ + (StaticBuffer::page_size() * 9);
}

void Fiber::start()
{
    *this_fiber() = this;

    context_ = new Context;

    // no code between these lines!
    // a nice side effect of how we store the context is that when we restore
    // the top of the stack will actually contain the return address of the
    // following line, in effect this allows us to resume from *after* the
    // following line, which is what we want otherwise we would call do_fiber
    // multiple times
    get_context(context_);
    do_start();

    // if we get here we have restored our context
}

void Fiber::finish()
{
    // decrement counter
    if (counter_ != nullptr)
    {
        (*counter_)--;
    }
}

void Fiber::suspend()
{
    (*Fiber::this_fiber())->set_state(FiberState::PAUSING);
    suspended_context_ = new Context;

    // preserve where we are and restore from the original context, this allows
    // us to resume later
    // like start() restoring suspended_context_ will continue from the
    // following line
    get_context(suspended_context_);
    do_suspend();

    // if we get here then we have been resumed, so rethrow any stored
    // exception
    if (exception_)
    {
        std::rethrow_exception(exception_);
    }
}

void Fiber::resume()
{
    state_ = FiberState::READY;
    *this_fiber() = this;

    do_resume();
}

Fiber **Fiber::this_fiber()
{
    // this allows us to get a pointer to the fiber being executed in the
    // current thread
    thread_local Fiber *current_fiber;
    return &current_fiber;
}

Counter *Fiber::counter()
{
    return counter_;
}

FiberState Fiber::state()
{
    return state_;
}

void Fiber::set_state(FiberState state)
{
    state_ = state;
}

/**
 * Starts the job. Once completed will restore the previously saved context.
 * If the Fiber has a parent (i.e. this is being waited on) and the job threw
 * an uncaught exception we rethrow it here.
 */
void Fiber::do_start()
{
    try
    {
        // swap to new stack and execute job
        change_stack(stack_);
        job_();
    }
    catch (...)
    {
        // catch all exceptions
        // if we have a parent i.e. someone is waiting on us then pass it the
        // exception
        // note we only do this if the parent isn't already storing an exception
        // it's a first-come first-served system
        if ((parent_fiber_ != nullptr) && (!parent_fiber_->has_exception_))
        {
            parent_fiber_->exception_ = std::current_exception();
            parent_fiber_->has_exception_ = true;
        }
        else
        {
            LOG_ENGINE_ERROR(
                "fiber", "another waiting fiber has already thrown");
        }
    }

    // job done, restore
    set_context(context_);
}

/**
 * Suspend the Fiber. This is a separate method so we can change state before
 * we change context and it doesn't mess up where we return to when restoring
 * context later.
 */
void Fiber::do_suspend()
{
    state_ = FiberState::RESUMABLE;
    set_context(context_);
}

/**
 * Resume the Fiber.
 *
 * @returns
 *   Always returns 0 - this is to prevent tail-call optimisation which
 *   can mess up the inlined assembly.
 */
int Fiber::do_resume()
{
    // store context so we can resume from here once our job has finished
    get_context(context_);
    set_context(suspended_context_);

    return 0;
}

}
