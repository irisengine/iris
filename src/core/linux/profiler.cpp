////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/profiler.h"

#include <filesystem>
#include <iostream>
#include <latch>
#include <memory>
#include <regex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <cxxabi.h>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

#include "core/auto_release.h"
#include "core/error_handling.h"
#include "core/profiler_analyser.h"
#include "core/thread.h"

namespace
{

// global state, needed as a signal handler is a global function
static constexpr auto stack_frame_size = 100u;
static std::vector<std::byte> proc_info_buffer(1024u * 1024u * 100u);
static const auto max_thread_count = std::thread::hardware_concurrency() * 10u;
static std::vector<void *> stack_traces;
static std::unordered_map<pid_t, std::size_t> index_lookup;
static std::unique_ptr<std::latch> threads_done;

/**
 * Custom signal handler that effectively allows us to suspend a thread.
 */
void signal_handler(int)
{
    // get the index into the global stack trace buffer we can write into, also sanity checks we are expecting to
    // profile this thread
    if (const auto iter = index_lookup.find(::gettid()); iter != std::cend(index_lookup))
    {
        // get stack trace
        const auto index = 100u * iter->second;
        const auto stack_size = ::backtrace(&stack_traces[index], 100u);

        // terminate the stack trace so we can find the end
        if (stack_size != 100u)
        {
            stack_traces[index + stack_size] = nullptr;
        }
    }

    // signal that this thread is done sampling
    threads_done->count_down();
}

}

namespace iris
{

struct Profiler::implementation
{
    Thread worker;
    std::atomic<bool> running;
};

Profiler::Profiler()
    : impl_(std::make_unique<implementation>())
{
    // register custom signal handler
    expect(::signal(SIGUSR1, &signal_handler) != SIG_ERR, "could not set signal handler");

    // reserve space for a stack frame for each thread
    stack_traces = std::vector<void *>(max_thread_count * stack_frame_size, nullptr);
    impl_->running = true;

    // ensure libgcc is initialised, if we don't do this here then the first call to backtrace might try to do the
    // initilisation which involves calls to malloc
    // if this happens from a suspended thread then it could cause a deadlock
    void *buffer = nullptr;
    expect(::backtrace(&buffer, 1u) == 1u, "failed to initialise libgcc");

    // create a new thread for handling the sampling, this thread will be excluded from the sampling
    impl_->worker = Thread([&]() {
        ProfilerAnalyser pa{};

        while (impl_->running)
        {
            std::vector<pid_t> tids{};

            // get all threads for the current process
            for (const auto &dir_entry : std::filesystem::directory_iterator{"/proc/self/task"})
            {
                const auto tid_str = dir_entry.path().filename().string();
                const auto tid = std::stoi(tid_str);
                tids.push_back(tid);
            }

            expect(tids.size() > 1, "found no threads");

            index_lookup.clear();

            // calculate indices for all the threads to write into
            auto index = 0u;
            for (const auto tid : tids)
            {
                index_lookup[tid] = index;
                ++index;
            }

            // we use a latch to know when all paused threads have resumed
            threads_done = std::make_unique<std::latch>(tids.size() - 1u);

            // DANGER ZONE START
            // as we don't know what a thread was doing when we suspended it we have to be careful what we do
            // we cannot allocate memory, most platform/system calls or anything which might involve trying to
            // take a lock that a suspended thread might be holding

            for (const auto tid : tids)
            {
                // skip the thread if it is the current thread, otherwise we will end up suspending ourselves
                if (tid == ::gettid())
                {
                    continue;
                }

                // send custom signal to the thread which will cause it to suspend
                ::syscall(SYS_tkill, tid, SIGUSR1);
            }

            threads_done->wait();

            // DANGER ZONE END

            std::regex symbol_regex{".*\\(([_a-zA-Z0-9]*).*"};

            // now that all threads have resumed we can resolve the symbols for all the stack traces
            for (auto i = 0u; i < stack_traces.size() / 100u; ++i)
            {
                std::vector<std::string> stack_trace{};
                auto index = i * 100u;

                const auto *begin = &stack_traces[index];
                const auto *end = &stack_traces[index + 100u];
                const auto size = std::find(begin, end, nullptr) - begin;

                // resolve stack trace to symbols
                AutoRelease<char **, nullptr> symbols(::backtrace_symbols(begin, size), ::free);

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
