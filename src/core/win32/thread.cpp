////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/thread.h"

#include <thread>

#include <Windows.h>

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
    DWORD affinity_mask = (1u << core);

    expect(::SetThreadAffinityMask(thread_.native_handle(), affinity_mask) != 0u, "could not bind thread to core");
}

}
