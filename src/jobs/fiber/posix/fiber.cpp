#include "jobs/fiber/fiber.h"

#include <atomic>
#include <cassert>
#include <cstddef>
#include <exception>
#include <memory>

#include "core/exception.h"
#include "core/static_buffer.h"
#include "jobs/context.h"
#include "jobs/job.h"
#include "log/log.h"

extern "C"
{
    // these will be defined with arch specific assembler
    extern void save_context(iris::Context *);
    extern void restore_context(iris::Context *);
    extern void change_stack(void *stack);
}

namespace iris
{

struct Fiber::implementation
{
    std::unique_ptr<StaticBuffer> stack_buffer;
    std::byte *stack;
    Context context;
    Context suspended_context;

    // all these functions are noinline and noopt
    // we require a very strict ordering of instructions in order to save and
    // restore contexts and we don't want the compiler to alter that

    /**
     * Starts a Fiber. Once completed will restore the previously saved context.
     *
     * @param fiber
     *   Fiber to start.
     */
    __attribute__((noinline, optnone)) static void do_start(Fiber *fiber)
    {
        try
        {
            // swap to new stack and execute job
            change_stack(fiber->impl_->stack);
            fiber->job_();
        }
        catch (...)
        {
            // store any exceptions so it can possibly be rethrown later
            if (fiber->exception_ == nullptr)
            {
                fiber->exception_ = std::current_exception();
            }
        }

        *this_fiber() = fiber->parent_fiber_;

        restore_context(&fiber->impl_->context);
    }

    /**
     * Suspend the Fiber.
     *
     * @param fiber
     *   Fiber to suspend.
     */
    __attribute__((noinline, optnone)) static void do_suspend(Fiber *fiber)
    {
        // no code between these lines
        // restoring this saved context will cause execution to continue from
        // *after* the following line
        save_context(&fiber->impl_->suspended_context);
        restore_context(&fiber->impl_->context);

        // the above call will not return
        // we get here when the suspended context is restored
    }

    /**
     * Resume the Fiber.
     *
     * @param fiber
     *   Fiber to resume.
     *
     * @returns
     *   Always returns 0 - this is to prevent tail-call optimisation which
     *   can mess up the assembly calls.
     */
    __attribute__((noinline, optnone)) static int do_resume(Fiber *fiber)
    {
        // store context so we can resume from here once our job has finished
        save_context(&fiber->impl_->context);
        restore_context(&fiber->impl_->suspended_context);

        return 0;
    }
};

Fiber::Fiber(Job job)
    : Fiber(job, nullptr)
{
}

Fiber::Fiber(Job job, Counter *counter)
    : job_(nullptr)
    , counter_(counter)
    , parent_fiber_(nullptr)
    , exception_(nullptr)
    , safe_(true)
    , impl_(std::make_unique<implementation>())
{
    job_ = job;

    impl_->stack_buffer = std::make_unique<StaticBuffer>(10u);

    // stack grows from high -> low memory so move our pointer down, not all the
    // way as we need some space to copy the previous stack frame
    impl_->stack = *impl_->stack_buffer + (StaticBuffer::page_size() * 9);
}

Fiber::~Fiber() = default;

void Fiber::start()
{
    // bookkeeping
    parent_fiber_ = *this_fiber();
    *this_fiber() = this;

    // save our context and kick off the job, we will return from here when the
    // job is done (but possible on a different thread)
    save_context(&impl_->context);
    implementation::do_start(this);

    if (!safe_)
    {
        // we are no longer suspending if we are here i.e. it is now safe for
        // another thread to pick us up
        safe_ = true;
    }
    else
    {
        // update counter if another fiber was waiting on us
        if (counter_ != nullptr)
        {
            (*counter_)--;
        }
    }
}

void Fiber::suspend()
{
    *this_fiber() = parent_fiber_;

    implementation::do_suspend(this);

    // if we get here then we have been resumed, so rethrow any stored
    // exception
    if (exception_)
    {
        std::rethrow_exception(exception_);
    }
}

void Fiber::resume()
{
    // bookkeeping
    parent_fiber_ = *this_fiber();
    *this_fiber() = this;

    implementation::do_resume(this);

    if (!safe_)
    {
        // we are no longer suspending if we are here i.e. it is now safe for
        // another thread to pick us up
        safe_ = true;
    }
    else
    {
        // update counter if another fiber was waiting on us
        if (counter_ != nullptr)
        {
            (*counter_)--;
        }
    }
}

bool Fiber::is_safe() const
{
    return safe_;
}

void Fiber::set_unsafe()
{
    safe_ = false;
}

bool Fiber::is_being_waited_on() const
{
    return counter_ != nullptr;
}

std::exception_ptr Fiber::exception() const
{
    return exception_;
}

void Fiber::thread_to_fiber()
{
    if (*this_fiber() != nullptr)
    {
        throw Exception("thread already a fiber");
    }

    // thread will clean this up when it ends
    *this_fiber() = new Fiber(nullptr);
}

Fiber **Fiber::this_fiber()
{
    // this allows us to get a pointer to the fiber being executed in the
    // current thread
    thread_local Fiber *current_fiber = nullptr;
    return &current_fiber;
}

}
