#pragma once

#include <type_traits>

#include <CoreFoundation/CoreFoundation.h>

namespace iris
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
class CfPtr
{
    public:

        /**
         * Construct a new CfPtr with the supplied object pointer.
         *
         * @param ptr
         *   Pointer to wrap (can be nullptr).
         */
        CfPtr(T ptr)
            : ptr_(ptr)
        { }

        /**
         * Release the CoreFoundation object.
         */
        ~CfPtr()
        {
            if(ptr_ != nullptr)
            {
                ::CFRelease(ptr_);
            }
        }

        /** Disabled. */
        CfPtr(const CfPtr&) = delete;
        CfPtr& operator=(const CfPtr&) = delete;

        /**
         * Move construct a new cf_ptr.
         *
         * Other must not be used after this call.
         *
         * @param other
         *   The CfPtr to construct from.
         */
        CfPtr(CfPtr &&other)
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
         *   The CfPtr to assign from.
         *
         * @returns
         *   Reference to this cf_ptr, after move assignment.
         */
        CfPtr& operator=(CfPtr &&other)
        {
            // create a new CfPtr object to 'steal' the internal state of the
            // supplied object
            CfPtr new_ptr{ std::move(other) };
            swap(new_ptr);

            return *this;
        }

        /**
         * Swap this CfPtr with another.
         *
         * @param other
         *   Object to swap with.
         */
        void swap(CfPtr &other)
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

