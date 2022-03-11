////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/exception.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"

namespace iris
{

/**
 * Class encapsulating a D3D12 structured buffer. Represents an array of fixed sized objects accessible by a shader.
 */
class D3D12StructuredBuffer
{
  public:
    /**
     * Construct a new D3D12StructuredBuffer.
     *
     * @param object_count
     *   Number of objects to allocate.
     *
     * @param object_size
     *   Size of object (in bytes).
     */
    D3D12StructuredBuffer(std::size_t object_count, std::size_t object_size);

    /**
     * Construct a new D3D12StructuredBuffer for a given frame, the internal descriptor handle will be recycled at the
     * end of the supplied frame.
     *
     * @param object_count
     *   Number of objects to allocate.
     *
     * @param object_size
     *   Size of object (in bytes).
     *
     * @param frame
     *   Frame to allocate descriptor handle for.
     */
    D3D12StructuredBuffer(std::size_t object_count, std::size_t object_size, std::uint32_t frame);

    ~D3D12StructuredBuffer();

    D3D12StructuredBuffer(const D3D12StructuredBuffer &) = delete;
    D3D12StructuredBuffer &operator=(const D3D12StructuredBuffer &) = default;
    D3D12StructuredBuffer(D3D12StructuredBuffer &&) = default;
    D3D12StructuredBuffer &operator=(D3D12StructuredBuffer &&) = default;

    /**
     * Get descriptor handle to buffer.
     *
     * @returns
     *   Buffer handle.
     */
    D3D12DescriptorHandle descriptor_handle() const;

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

        std::memcpy(mapped_buffer_ + offset, object, size);
    }

    /**
     * Get D3D12 resource.
     *
     * @returns
     *   Internal resource for buffer.
     */
    ID3D12Resource *resource() const
    {
        return resource_.Get();
    }

  private:
    /** Capacity (in bytes) of buffer. */
    std::size_t capacity_;

    /** Pointer to mapped buffer where data can be written. */
    std::byte *mapped_buffer_;

    /** D3D12 handle to resource view. */
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

    /** D3D12 handle to buffer. */
    D3D12DescriptorHandle descriptor_handle_;

    /** Flag indicating if the descriptor handle is static (or dynamic per frame). */
    bool static_descriptor_;
};

}
