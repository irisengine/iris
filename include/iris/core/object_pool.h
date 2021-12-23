////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/error_handling.h"
#include <cstddef>

namespace iris
{

/**
 * Simple generic object pool class.
 *
 * Internally this allocates a large continuous block for all objects and returns/releases objects from the end.
 */
template <class T, std::size_t N = 1000, class Allocator = std::allocator<T>>
class ObjectPool
{
  public:
    /**
     * Construct a new ObjectPool.
     *
     * @param alloc
     *   Allocator for object in pool.
     */
    ObjectPool(const Allocator &alloc = Allocator())
        : alloc_(alloc)
        , head_(alloc_.allocate(N))
        , next_(head_ + N)
    {
    }

    /**
     * Frees the internal pool memory. Note that any objects not passed to release() will *not* have their destructor
     * called.
     */
    ~ObjectPool()
    {
        alloc_.deallocate(head_, N);
    }

    /**
     * Get the next object from the pool. Will construct it with the supplied args.
     *
     * @param args
     *   Argument to forward to the object constructor.
     *
     * @returns
     *   Next object.
     */
    template <class... Args>
    T *next(Args &&...args)
    {
        ensure(next_ != head_, "object pool drained");

        // get next object and construct it
        auto *object = next_;
        std::construct_at(object, std::forward<Args>(args)...);

        // move the next pointer to the next object to return
        --next_;

        return object;
    }

    /**
     * Returns an object to the pool.
     *
     * @param object
     *   Object to return to pool.
     */
    void release(T *object)
    {
        // call the destructor of the returned object
        std::destroy_at(object);

        // move the next pointer to the next object to return
        ++next_;
    }

  private:
    /** The allocator to use of T. */
    Allocator alloc_;

    /** The front of the object pool. */
    T *head_;

    /** Pointer into the pool of the next object to return. */
    T *next_;
};

}
