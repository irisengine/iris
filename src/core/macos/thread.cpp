#include "core/thread.h"

#include <thread>

#include <mach/mach.h>
#include <mach/thread_policy.h>
#include <pthread.h>

#include "core/error_handling.h"

namespace iris
{

Thread::Thread()
    : thread_()
{
}

bool Thread::joinable() const
{
    return thread_.joinable();
}

void Thread::join()
{
    thread_.join();
}

std::thread::id Thread::get_id() const
{
    return thread_.get_id();
}

void Thread::bind_to_core(std::size_t core)
{
    ensure(core < std::thread::hardware_concurrency(), "invalid core id");

    // convert pthread to mach_thread
    thread_affinity_policy_data_t policy = {static_cast<int>(core)};
    const auto mach_thread = pthread_mach_thread_np(thread_.native_handle());

    // set affinity policy, this is merely a suggestion to the kernel
    const auto set_policy = ::thread_policy_set(
        mach_thread,
        THREAD_AFFINITY_POLICY,
        reinterpret_cast<thread_policy_t>(&policy),
        1);
    expect(set_policy == KERN_SUCCESS, "failed to bind thread to core");
}

}
