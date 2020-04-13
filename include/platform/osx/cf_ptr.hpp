#pragma once

#include <type_traits>

#include <CoreFoundation/CoreFoundation.h>

namespace eng
{

/**
 * A RAII wrapper for CoreFoundation objects. This object is analogous to a
 * std::unique_ptr and will release the resource when it goes out of scope.
 *
 * This class is needed as all CoreFoundation objects are opaque types deriving
 * from CFType and are all released with CFRelease, making them cumbersome to
 * use directly with unique_ptr.
 */
template<
    class T,
    typename=typename std::enable_if_t<std::is_pointer<T>::value>>
class cf_ptr
{
    public:

        /**
         * Construct a new cf_ptr with the supplied object pointer.
         *
         * @param ptr
         *   Pointer to wrap (can be nullptr).
         */
        cf_ptr(T ptr)
            : ptr_(ptr)
        { }

        /**
         * Release the CoreFoundation object.
         */
        ~cf_ptr()
        {
            if(ptr_ != nullptr)
            {
                ::CFRelease(ptr_);
            }
        }

        /** Disabled. */
        cf_ptr(const cf_ptr&) = delete;
        cf_ptr& operator=(const cf_ptr&) = delete;

        /**
         * Move construct a new cf_ptr.
         *
         * Other must not be used after this call.
         *
         * @param other
         *   The cf_ptr to construct from.
         */
        cf_ptr(cf_ptr &&other)
            : ptr_(nullptr)
        {
            swap(other);
        }

        /**
         * Move assign a new cf_ptr.
         *
         * Other must not be used after this call.
         *
         * @param other
         *   The cf_ptr to assign from.
         *
         * @returns
         *   Reference to this cf_ptr, after move assignment.
         */
        cf_ptr& operator=(cf_ptr &&other)
        {
            // create a new cf_ptr object to 'steal' the internal state of the
            // supplied object
            cf_ptr new_ptr{ std::move(other) };
            swap(new_ptr);

            return *this;
        }

        /**
         * Swap this cf_ptr with another.
         *
         * @param other
         *   Object to swap with.
         */
        void swap(cf_ptr &other)
        {
            std::swap(ptr_, other.ptr_);
        }

        /**
         * Check whether this owns a CoreFoundation object.
         *
         * @returns
         *   True if this owns a CoreFoundation object, false otherwise.
         */
        operator bool() const
        {
            return ptr_ != nullptr;
        }

        /**
         * Get the managed pointer. This should be used as a 'peek' operation,
         * the returned pointer should never be passed to CFRetain.
         *
         * @returns
         *   The managed pointer.
         */
        T get() const
        {
            return ptr_;
        };

    private:

        /** Managed pointer. */
        T ptr_;
};

}

