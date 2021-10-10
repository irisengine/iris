////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>

#import <Metal/Metal.h>

#include "core/exception.h"

namespace iris
{

/**
 * This class encapsulates a constant shader buffer. This is data that is set
 * once then made available to all vertices/fragments. It is analogous to an
 * OpenGL uniform.
 */
class MetalConstantBuffer
{
  public:
    /**
     * Construct a new MetalConstantBuffer.
     *
     * @param capacity
     *   Size (in bytes) of buffer.
     */
    MetalConstantBuffer(std::size_t capacity);

    MetalConstantBuffer(const MetalConstantBuffer &) = delete;
    MetalConstantBuffer &operator=(const MetalConstantBuffer &) = delete;

    /**
     * Write an object into the buffer at an offset.
     *
     * @param object
     *   Object to write.
     *
     * @param offset
     *   Offset into buffer to write object.
     */
    template <class T>
    void write(const T &object, std::size_t offset)
    {
        write(std::addressof(object), sizeof(T), offset);
    }

    /**
     * Write an object into the buffer at an offset.
     *
     * @param object
     *   Object to write.
     *
     * @param size
     *   Size (in bytes) of object to write.
     *
     * @param offset
     *   Offset into buffer to write object.
     */
    template <class T>
    void write(const T *object, std::size_t size, std::size_t offset)
    {
        if (offset + size > capacity_)
        {
            throw Exception("write would overflow");
        }

        std::memcpy(static_cast<std::byte *>(buffer_.contents) + offset, object, size);
    }

    /**
     * Get metal handle to buffer.
     *
     * @returns
     *   Metal handle.
     */
    id<MTLBuffer> handle() const;

  private:
    /** Metal handle to buffer. */
    id<MTLBuffer> buffer_;

    /** Capacity (in bytes) of buffer. */
    std::size_t capacity_;
};

}
