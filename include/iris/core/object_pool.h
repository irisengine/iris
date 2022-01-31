////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <memory>
#include <mutex>

#include "core/error_handling.h"
#include "log/log.h"

namespace iris
{

/**
 * Simple thread-safe generic object pool class.
 *
 * Internally this allocates a large continuous block for all objects and returns/releases objects from the start.
 */
template <class T, std::size_t N = 50000, class Allocator = std::allocator<T>>
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
        , free_list_alloc_()
        , objects_(nullptr)
        , free_list_(nullptr)
        , next_(nullptr)
    {
        static_assert(N > 0);

        // create the fixed array of objects and a free list
        // the free list is an array of pointers where each element points to an object on the pool
        // the next_ pointer tracks which will be the next object to be given out from the pool
        objects_ = alloc_.allocate(N);

        try
        {
            // ensure that if allocating the free list throws we don't leak the object pool
            free_list_ = free_list_alloc_.allocate(N);
        }
        catch (...)
        {
            alloc_.deallocate(objects_, N);
            throw;
        }

        auto *obj_cursor = objects_;
        auto **free_cursor = free_list_;

        // wire up the free list so each pointer is pointing to an object in the pool
        while (free_cursor != free_list_ + N)
        {
            *free_cursor = obj_cursor;
            ++free_cursor;
            ++obj_cursor;
        }

        // start handing out objects from the start of the free list
        next_ = free_list_;
    }

    /**
     * Frees the internal pool memory. Note that any objects not passed to release() will *not* have their destructor
     * called.
     */
    ~ObjectPool()
    {
        alloc_.deallocate(objects_, N);
        free_list_alloc_.deallocate(free_list_, N);
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
        ensure(next_ != free_list_ + N, "object pool drained");

        T *object = nullptr;

        // get the next object and update the next pointer
        {
            std::unique_lock lock(mutex_);
            object = *next_;
            ++next_;
        }

        ::new (object) T(std::forward<Args>(args)...);

        return object;
    }

    /**
     * Returns an object to the pool.
     *
     * @param object
     *   Object to return to pool.
     */
    void release(const T *object)
    {
        // call the destructor of the returned object
        std::destroy_at(object);

        // return the object to the pool and move the next pointer back
        {
            std::unique_lock lock(mutex_);
            --next_;
            *next_ = const_cast<T *>(object);
        }
    }

  private:
    /** The allocator to use of T. */
    Allocator alloc_;

    /** Rebound allocator for allocating T* */
    typename std::allocator_traits<Allocator>::template rebind_alloc<T *> free_list_alloc_;

    /** Pool of allocated objects. */
    T *objects_;

    /** Array of pointers to allocated objects. */
    T **free_list_;

    /** Pointer into free list for next object to return. */
    T **next_;

    /** Mutex to ensure thread safety. */
    std::mutex mutex_;
};

}
