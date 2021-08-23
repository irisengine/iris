#pragma once

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/exception.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_cpu_descriptor_handle_allocator.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/d3d12/d3d12_descriptor_manager.h"
#include "graphics/d3d12/d3d12_gpu_descriptor_handle_allocator.h"
#include "graphics/texture_manager.h"

namespace iris
{

/**
 * This class encapsulates a constant shader buffer. This is data that is set
 * once then made available to all vertices/fragments. It is analogous to an
 * OpenGL uniform.
 */
class D3D12ConstantBuffer
{
  public:
    /**
     * Construct a null D3D12ConstantBuffer.
     */
    D3D12ConstantBuffer();

    /**
     * Construct a new D3D12ConstantBuffer.
     *
     * @param capacity
     *   Size (in bytes) of buffer.
     */
    D3D12ConstantBuffer(std::uint32_t capacity);

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

  private:
    /** Capacity (in bytes) of buffer. */
    std::uint32_t capacity_;

    /** Pointer to mapped buffer where data can be written. */
    std::byte *mapped_buffer_;

    /** D3D12 handle to resource view. */
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

    /** D3D12 handle to buffer. */
    D3D12DescriptorHandle descriptor_handle_;
};

}
