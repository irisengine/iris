////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/profiler.h"

#include <atomic>
#include <cstddef>
#include <memory>
#include <numeric>
#include <regex>
#include <thread>
#include <vector>

#include <cxxabi.h>
#include <execinfo.h>
#include <mach/mach.h>
#include <mach/mach_traps.h>
#include <mach/task.h>
#include <unistd.h>

#include "core/error_handling.h"
#include "core/profiler_analyser.h"
#include "core/thread.h"

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
    AutoSuspendThread(thread_act_t thread)
        : thread_(thread)
        , suspend_result_(::thread_suspend(thread_))
    {
    }

    /**
     * Bool operator to check if thread was suspended.
     *
     * @returns
     *   True if thread was suspended, otherwise false.
     */
    explicit operator bool() const
    {
        return suspend_result_ == KERN_SUCCESS;
    }

    /**
     * Get the result of suspending the thread.
     *
     * @returns
     *   Thread suspend result.
     */
    kern_return_t suspend_result() const
    {
        return suspend_result_;
    }

    /**
     * Resume the thread (if it was suspended).
     */
    ~AutoSuspendThread()
    {
        if (*this)
        {
            ::thread_resume(thread_);
        }
    }

  private:
    /** Handle to thread to suspend/resume. */
    thread_act_t thread_;

    /** Result of suspending the thread. */
    kern_return_t suspend_result_;
};

}

namespace iris
{

struct Profiler::implementation
{
    Thread worker;
    std::atomic<bool> running;
    std::vector<void *> stack_traces;
};

Profiler::Profiler()
    : impl_(std::make_unique<implementation>())
{
    // reserve space for a stack frame for each thread
    impl_->stack_traces.resize(max_thread_count * stack_frame_size);
    impl_->running = true;

    // create a new thread for handling the sampling, this thread will be excluded from the sampling
    impl_->worker = Thread([&]() {
        ProfilerAnalyser pa{};

        while (impl_->running)
        {
            // get current task
            task_t t;
            expect(::task_for_pid(mach_task_self(), ::getpid(), &t) == KERN_SUCCESS, "could not get task");

            // get all threads for task
            thread_act_array_t thread_list;
            mach_msg_type_number_t thread_count = 0u;
            expect(::task_threads(t, &thread_list, &thread_count) == KERN_SUCCESS, "could not get threads");

            const auto loop_limit = std::min(thread_count, max_thread_count);

            for (auto i = 0u; i < loop_limit; ++i)
            {
                const auto thread = thread_list[i];

                // skip the thread if it is the current thread, otherwise we will end up suspending ourselves
                if (thread == mach_thread_self())
                {
                    continue;
                }

                const auto index = i * stack_frame_size;

                impl_->stack_traces[index] = nullptr;

                // suspend the thread
                AutoSuspendThread auto_suspend(thread);

                // DANGER ZONE START
                // as we don't know what a thread was doing when we suspended it we have to be careful what we do
                // we cannot allocate memory, most platform/system calls or anything which might involve trying to
                // take a lock that a suspended thread might be holding

                if (!auto_suspend)
                {
                    continue;
                }

// arch specific structs
#if defined(IRIS_ARCH_ARM64)
                arm_thread_state64_t state;
                mach_msg_type_number_t state_count = ARM_THREAD_STATE64_COUNT;
                const auto flavour = ARM_THREAD_STATE64;
#elif defined(IRIS_ARCH_X86_64)
                x86_thread_state64_t state;
                mach_msg_type_number_t state_count = x86_THREAD_STATE64_COUNT;
                const auto flavour = x86_THREAD_STATE64;
#else
#error unsupported architecture
#endif

                // get the state of the thread
                if (::thread_get_state(thread, flavour, reinterpret_cast<thread_state_t>(&state), &state_count) !=
                    KERN_SUCCESS)
                {
                    continue;
                }

#if defined(IRIS_ARCH_ARM64)
                // get the stack trace
                const auto stack_size = ::backtrace_from_fp(
                    reinterpret_cast<void *>(state.__fp), &impl_->stack_traces[index], stack_frame_size);
#elif defined(IRIS_ARCH_X86_64)
                // get the stack trace
                const auto stack_size = ::backtrace_from_fp(
                    reinterpret_cast<void *>(state.__rbp), &impl_->stack_traces[index], stack_frame_size);
#else
#error unsupported architecture
#endif

                if (stack_size < stack_frame_size)
                {
                    impl_->stack_traces[stack_size] = nullptr;
                }

                // DANGER ZONE END
            }

            // now that all threads have resumed we can resolve the symbols for all the stack traces

            std::regex symbol_regex{"[0-9]+\\s+[^\\s]*\\s+0x[0-9a-fA-f]*\\s([^\\s]+).*"};

            for (auto i = 0u; i < loop_limit; ++i)
            {
                std::vector<std::string> stack_trace{};
                const auto index = i * stack_frame_size;
                const auto begin = std::cbegin(impl_->stack_traces) + index;
                const auto end = std::find(begin, begin + stack_frame_size, nullptr);
                const auto size = std::distance(begin, end);

                // resolve stack trace to symbols
                AutoRelease<char **, nullptr> symbols(::backtrace_symbols(std::addressof(*begin), size), ::free);

                // try and demangle each symbol
                for (auto i = 0u; i < size; ++i)
                {
                    if (const auto *symbol = symbols[i]; symbol != nullptr)
                    {
                        std::cmatch cmatch{};
                        std::string symbol_str = "unknown";

                        if (std::regex_match(symbol, cmatch, symbol_regex))
                        {
                            if ((cmatch.size() == 2u) && (cmatch[1].length() > 0u))
                            {
                                AutoRelease<char *, nullptr> auto_demangle(
                                    ::abi::__cxa_demangle(cmatch[1].str().c_str(), nullptr, nullptr, nullptr), ::free);
                                ;
                                if (auto_demangle)
                                {
                                    symbol_str = auto_demangle;
                                }
                            }
                        }

                        stack_trace.push_back(symbol_str);
                    }
                    else
                    {
                        break;
                    }
                }

                // record the resolved stack trace
                pa.add_stack_trace(stack_trace);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
