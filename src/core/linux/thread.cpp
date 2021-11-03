////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/thread.h"

#include <thread>

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

    const auto pthread_handle = thread_.native_handle();

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);

    ensure(::pthread_setaffinity_np(pthread_handle, sizeof(cpuset), &cpuset) == 0, "could not set cputset");
}

}
