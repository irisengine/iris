////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>
#include <type_traits>

#include "concepts/is_pointer.h"

namespace iris
{

/**
 * Generic RAII class for taking ownership of a resource and releasing it when
 * this class goes out of scope.
 */
template <class T, T Invalid>
class AutoRelease
{
  public:
    /**
     * Construct a new AutoRelease which doesn't own any object.
     */
    AutoRelease()
        : AutoRelease(Invalid, nullptr)
    {
    }

    /**
     * Construct a new AutoRelease which takes ownership of a resource
     *
     * @param resource
     *   Resource to own
     *
     * @param deleter
     *   Function to release resource at end of scope.
     */
    AutoRelease(T resource, std::function<void(T)> deleter)
        : resource_(resource)
        , deleter_(deleter)
    {
    }

    /**
     * Release resource with supplied deleter.
     */
    ~AutoRelease()
    {
        if ((resource_ != Invalid) && deleter_)
        {
            deleter_(resource_);
        }
    }

    /**
     * Move constructor. Steals ownership from supplied object.
     *
     * @param other
     *   Object to move construct from.
     */
    AutoRelease(AutoRelease<T, Invalid> &&other) noexcept
        : AutoRelease(Invalid, {})
    {
        swap(other);
    }

    /**
     * Move assignment. Steals ownership from supplied object.
     *
     * @param other
     *   Object to move assign from.
     */
    AutoRelease<T, Invalid> &operator=(AutoRelease<T, Invalid> &&other)
    {
        AutoRelease<T, Invalid> new_auto_release{std::move(other)};
        swap(new_auto_release);

        return *this;
    }

    AutoRelease(const AutoRelease<T, Invalid> &) = delete;
    AutoRelease<T, Invalid> &operator=(AutoRelease<T, Invalid> &) = delete;

    /**
     * Swap this object with another.
     *
     * @param other
     *   Object to swap with.
     */
    void swap(AutoRelease<T, Invalid> &other) noexcept
    {
        std::swap(resource_, other.resource_);
        std::swap(deleter_, other.deleter_);
    }

    /**
     * Get the managed resource.
     *
     * @returns
     *   Managed resource.
     */
    T get() const noexcept
    {
        return resource_;
    }

    /**
     * Get the address of the internally managed resource. This is useful if we
     * are managing a pointer and need to pass that to another function to get
     * set.
     *
     * @returns
     *   Address of managed resource.
     */
    T *operator&() noexcept
    {
        return std::addressof(resource_);
    }

    /**
     * Overload arrow operator (for pointer objects only).
     *
     * @returns
     *   Copy of resource (will be a pointer).
     */
    T operator->() const noexcept requires IsPointer<T>
    {
        return resource_;
    }

    /**
     * Get if this object manages a resource.
     *
     * @returns
     *   True if this object managed a resource, false otherwise.
     */
    explicit operator bool() const noexcept
    {
        return resource_ != Invalid;
    }

    /**
     * Cast operator.
     *
     * @returns
     *   Managed resource.
     */
    operator T() const noexcept
    {
        return resource_;
    }

  private:
    /** Managed resource. */
    T resource_;

    /** Resource delete function. */
    std::function<void(T)> deleter_;
};

}
