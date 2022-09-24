////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/profiler.h"

#include <atomic>
#include <cstddef>
#include <memory>
#include <thread>
#include <vector>

#include "Windows.h"
#include "dbghelp.h"
#include "winternl.h"

#include "core/error_handling.h"
#include "core/profiler_analyser.h"
#include "core/thread.h"

#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "ntdll.lib")

namespace
{

static constexpr auto stack_frame_size = 100u;
static std::vector<std::byte> proc_info_buffer(1024u * 1024u * 100u);
static const auto max_thread_count = std::thread::hardware_concurrency() * 10u;

/**
 * Helper RAII class to suspend thread on construction and resume it on destruction
 */
class AutoSuspendThread
{
  public:
    /**
     * Construct a new AutoSuspendThread.
     *
     * @param thread
     *   Thread to suspend
     */
    AutoSuspendThread(HANDLE thread)
        : thread_(thread)
        , suspend_result_(::SuspendThread(thread_))
    {
    }

    /**
     * Get the result of suspending the thread.
     *
     * @returns
     *   Thread suspend result.
     */
    DWORD suspend_result() const
    {
        return suspend_result_;
    }

    /**
     * Resume the thread (if it was suspended).
     */
    ~AutoSuspendThread()
    {
        if (suspend_result_ != -1)
        {
            ::ResumeThread(thread_);
        }
    }

  private:
    /** Handle to thread to suspend/resume. */
    HANDLE thread_;

    /** Result of suspending the thread. */
    DWORD suspend_result_;
};

/**
 * Helper function to get all threads for the current process.
 *
 * @returns
 *   Collection of all thread handles for current process, or an empty optional on failure.
 */
std::optional<std::vector<HANDLE>> threads_for_current_process()
{
    std::optional<std::vector<HANDLE>> handles{};

    ULONG return_length = 0u;

    // get all process information
    if (NT_SUCCESS(::NtQuerySystemInformation(
            SystemProcessInformation,
            proc_info_buffer.data(),
            static_cast<ULONG>(proc_info_buffer.size()),
            &return_length)) == TRUE)
    {
        std::size_t proc_index = 0u;
        std::size_t offset = 0u;

        iris::ensure(return_length <= proc_info_buffer.size(), "buffer too small");

        // walk through the buffer of all process information looking for the one for this process
        do
        {
            proc_index += offset;

            const auto *proc_info = reinterpret_cast<const SYSTEM_PROCESS_INFORMATION *>(&proc_info_buffer[proc_index]);
            const auto pid = reinterpret_cast<HANDLE>(static_cast<std::uint64_t>(::GetCurrentProcessId()));

            // check if the current process is ours
            if (proc_info->UniqueProcessId == pid)
            {
                // thread information comes straight after the process information
                std::size_t thread_index = proc_index + sizeof(SYSTEM_PROCESS_INFORMATION);
                const auto *thread_info_array =
                    reinterpret_cast<const SYSTEM_THREAD_INFORMATION *>(&proc_info_buffer[thread_index]);

                handles = std::vector<HANDLE>();
                handles->reserve(proc_info->NumberOfThreads);

                // copy all thread handles
                for (auto i = 0u; i < proc_info->NumberOfThreads; ++i)
                {
                    const auto &thread_info = thread_info_array[i];

                    iris::expect(thread_info.ClientId.UniqueProcess == pid, "wrong process id");

                    // despite being a HANDLE the UniqueThread field is actually a thread id, so we need to resolve
                    // that back to a handle
                    auto handle = ::OpenThread(
                        THREAD_ALL_ACCESS,
                        FALSE,
                        static_cast<DWORD>(reinterpret_cast<std::ptrdiff_t>(thread_info.ClientId.UniqueThread)));

                    handles->push_back(handle);
                }

                break;
            }

            offset = proc_info->NextEntryOffset;

        } while (offset != 0u);
    }

    return handles;
}

}

namespace iris
{

struct Profiler::implementation
{
    Thread worker;
    std::atomic<bool> running;
    std::vector<DWORD64> stack_traces;
};

Profiler::Profiler()
    : impl_(std::make_unique<implementation>())
{
    // ensure we can resolve symbols
    ensure(::SymInitialize(::GetCurrentProcess(), NULL, TRUE) == TRUE, "failed to init symbol handler");

    // reserve space for a stack frame for each thread
    impl_->stack_traces.resize(max_thread_count * stack_frame_size);
    impl_->running = true;

    // create a new thread for handling the sampling, this thread will be excluded from the sampling
    impl_->worker = Thread([&]() {
        ProfilerAnalyser pa{};

        while (impl_->running)
        {
            // get all threads for the current process
            if (const auto &thread_handles = threads_for_current_process(); thread_handles)
            {
                auto i = 0u;
                for (const auto &handle : *thread_handles)
                {
                    // skip the thread if it is the current thread, otherwise we will end up suspending ourselves
                    if (::GetThreadId(handle) == ::GetThreadId(::GetCurrentThread()))
                    {
                        continue;
                    }

                    // suspend the thread
                    AutoSuspendThread auto_suspend(handle);

                    // DANGER ZONE START
                    // as we don't know what a thread was doing when we suspended it we have to be careful what we do
                    // we cannot allocate memory, most platform/system calls or anything which might involve trying to
                    // take a lock that a suspended thread might be holding

                    if (auto_suspend.suspend_result() == -1)
                    {
                        break;
                    }

                    CONTEXT context = {.ContextFlags = CONTEXT_FULL};
                    if (::GetThreadContext(handle, &context) == 0)
                    {
                        break;
                    }

                    // build the STACKFRAME64 object based on the thread context
                    STACKFRAME64 stack_frame = {
                        .AddrPC = {.Offset = context.Rip, .Mode = AddrModeFlat},
                        .AddrFrame = {.Offset = context.Rbp, .Mode = AddrModeFlat},
                        .AddrStack = {.Offset = context.Rsp, .Mode = AddrModeFlat}};

                    // get the stack trace for the thread
                    auto index = i++ * 100u;
                    while (::StackWalk64(
                               IMAGE_FILE_MACHINE_AMD64,
                               ::GetCurrentProcess(),
                               handle,
                               &stack_frame,
                               &context,
                               NULL,
                               ::SymFunctionTableAccess64,
                               ::SymGetModuleBase64,
                               NULL) == TRUE)
                    {
                        impl_->stack_traces[index] = stack_frame.AddrPC.Offset;
                        ++index;

                        if (index == 100u)
                        {
                            break;
                        }
                    }

                    // terminate the stack trace so we can find the end
                    if (index != 100u)
                    {
                        impl_->stack_traces[index] = 0u;
                    }

                    // DANGER ZONE END
                }
            }

            // now that all threads have resumed we can resolve the symbols for all the stack traces
            for (auto i = 0u; i < max_thread_count; ++i)
            {
                std::vector<std::string> stack_trace{};
                auto index = i * 100u;

                while (impl_->stack_traces[index] != 0u)
                {
                    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
                    auto *symbol_info = reinterpret_cast<SYMBOL_INFO *>(buffer);
                    symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
                    symbol_info->MaxNameLen = MAX_SYM_NAME;
                    DWORD64 displacement = 0u;

                    if (::SymFromAddr(::GetCurrentProcess(), impl_->stack_traces[index], &displacement, symbol_info) ==
                        TRUE)
                    {
                        stack_trace.emplace_back(symbol_info->Name, symbol_info->NameLen);
                    }
                    else
                    {
                        stack_trace.push_back("unknown");
                    }

                    ++index;
                }

                // record the resolved stack trace
                pa.add_stack_trace(stack_trace);
            }

            ::Sleep(10);
        }

        pa.print();
    });
}

Profiler::~Profiler()
{
    impl_->running = false;
    impl_->worker.join();
}
}
