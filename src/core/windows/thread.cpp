#include "core/thread.h"

#include <thread>

#include <Windows.h>

#include "core/exception.h"

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
    if (::SetThreadAffinityMask(thread_.native_handle(), affinity_mask) == 0)
    {
        throw Exception("could not bind thread to core");
    }
}

}
