////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>

namespace iris
{

/**
 * A syntonisation primitive which contains a counter that can be incremented
 * (release) or decremented (acquire) by any thread. Attempting to acquire a
 * semaphore when the internal count is 0 will block the calling thread until
 * another calls release.
 */
class Semaphore
{
  public:
    /**
     * Create a new Semaphore with an initial value for the counter.
     *
     * @param initial
     *   Initial value for counter.
     */
    Semaphore(std::ptrdiff_t initial = 0);

    ~Semaphore();

    Semaphore(const Semaphore &) = delete;
    Semaphore &operator=(const Semaphore &) = delete;
    Semaphore(Semaphore &&);
    Semaphore &operator=(Semaphore &&);

    /**
     * Increment counter and unblock any waiting threads.
     */
    void release();

    /**
     * Decrement counter or block until it can.
     */
    void acquire();

  private:
    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
