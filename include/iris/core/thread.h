////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <thread>
#include <utility>

namespace iris
{

/**
 * Class encapsulating a thread. In general this is an internal class. For
 * parallelising work the JobSystem should be used.
 */
class Thread
{
  public:
    /**
     * Construct a new thread not associated with any function.
     */
    Thread();

    /**
     * Construct a thread and run the supplied function and args.
     *
     * @param function
     *   Function to run in thread.
     *
     * @param args
     *   Arguments of function, will be perfectly forwarded.
     */
    template <class Function, class... Args>
    Thread(Function &&function, Args &&...args)
        : thread_(std::forward<Function>(function), std::forward<Args>(args)...)
    {
    }

    /**
     * Checks if this thread is currently active.
     *
     * @returns
     *   True if this thread has started and not yet been joined, else
     *   False.
     */
    bool joinable() const;

    /**
     * Block and wait for this thread to finish executing.
     */
    void join();

    /**
     * Get id of thread.
     *
     * @returns
     *   Thread id.
     */
    std::thread::id get_id() const;

    /**
     * Bind this thread such that it only executes on the specified core,
     * preventing the kernel from scheduling it onto another core.
     *
     * Note that depending on the current platform this may act as a
     * suggestion to the kernel, rather than be honoured.
     *
     * @param core
     *   Id of core to bind to in the range [0, number of cores)
     */
    void bind_to_core(std::size_t core);

  private:
    /** Internal thread object. */
    std::thread thread_;
};

}
