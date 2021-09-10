#include "jobs/fiber/fiber.h"

#include <cassert>
#include <exception>

#include <Windows.h>

#include "core/auto_release.h"
#include "core/exception.h"
#include "jobs/job.h"

namespace iris
{

// we disable optimisations for job_runner as the inlining messes up with the
// fiber resuming code
#pragma optimize("", off)
struct Fiber::implementation
{
    AutoRelease<LPVOID, NULL> handle;

    /**
     * Start function for win32 fiber.
     *
     * @param data
     *   Data passed to start function.
     */
    static void job_runner(void *data)
    {

        auto *fiber = static_cast<Fiber *>(data);
        *this_fiber() = fiber;

        try
        {
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

        fiber->parent_fiber_->resume();
    }
};
#pragma optimize("", on)

Fiber::Fiber(Job job)
    : Fiber(job, nullptr)
{
}

Fiber::Fiber(Job job, Counter *counter)
    : job_()
    , counter_(counter)
    , parent_fiber_(nullptr)
    , exception_(nullptr)
    , safe_(true)
    , impl_(std::make_unique<Fiber::implementation>())
{
    job_ = job;
    impl_->handle = {
        ::CreateFiberEx(
            0,
            0,
            FIBER_FLAG_FLOAT_SWITCH,
            implementation::job_runner,
            static_cast<void *>(this)),
        ::DeleteFiber};

    if (!impl_->handle)
    {
        throw Exception("create fiber failed");
    }
}

Fiber::~Fiber() = default;

void Fiber::start()
{
    // bookkeeping
    parent_fiber_ = *this_fiber();
    *this_fiber() = this;

    // switch to fiber (this will kick-off the job)
    ::SwitchToFiber(impl_->handle);

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
            --(*counter_);
        }
    }
}

void Fiber::suspend()
{
    *this_fiber() = parent_fiber_;

    ::SwitchToFiber(parent_fiber_->impl_->handle);

    // if we get here then we have been resumed, so rethrow any stored
    // exception
    if (exception_ != nullptr)
    {
        std::rethrow_exception(exception_);
    }
}

void Fiber::resume()
{
    // bookkeeping
    parent_fiber_ = *this_fiber();
    *this_fiber() = this;

    ::SwitchToFiber(impl_->handle);

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
            --(*counter_);
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
    auto *fiber = new Fiber{nullptr, nullptr};

    fiber->impl_->handle = {
        ::ConvertThreadToFiberEx(NULL, FIBER_FLAG_FLOAT_SWITCH),
        [fiber](auto) { ::ConvertFiberToThread(); }};

    if (!fiber->impl_->handle)
    {
        throw Exception("convert thread to fiber failed");
    }

    *this_fiber() = fiber;
}

Fiber **Fiber::this_fiber()
{
    // this allows us to get a pointer to the fiber being executed in the
    // current thread
    thread_local Fiber *this_fiber = nullptr;
    return &this_fiber;
}

}
