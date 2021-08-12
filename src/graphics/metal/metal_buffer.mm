#include "graphics/metal/metal_buffer.h"

#include <cstddef>
#include <cstdint>
#include <vector>

#import <Metal/Metal.h>

#include "core/macos/macos_ios_utility.h"
#include "graphics/vertex_data.h"

namespace
{

/**
 * Helper function to create a metal Buffer from a collection of objects.
 *
 * @param data
 *   Data to store on buffer.
 *
 * @returns
 *   Handle to metal buffer.
 */
template <class T>
id<MTLBuffer> create_buffer(const std::vector<T> &data)
{
    auto *device = iris::core::utility::metal_device();

    // create buffer with data
    return [device newBufferWithBytes:static_cast<const void *>(data.data())
                               length:sizeof(T) * data.size()
                              options:MTLResourceOptionCPUCacheModeDefault];
}

}

namespace iris
{

MetalBuffer::MetalBuffer(const std::vector<VertexData> &vertex_data)
    : handle_(create_buffer(vertex_data))
    , element_count_(vertex_data.size())
    , capacity_(vertex_data.size())
{
}

MetalBuffer::MetalBuffer(const std::vector<std::uint32_t> &index_data)
    : handle_(create_buffer(index_data))
    , element_count_(index_data.size())
    , capacity_(index_data.size())
{
}

id<MTLBuffer> MetalBuffer::handle() const
{
    return handle_;
}

std::size_t MetalBuffer::element_count() const
{
    return element_count_;
}

void MetalBuffer::write(const std::vector<VertexData> &vertex_data)
{
    // if the new data is larger than existing buffer then allocate a new,
    // larger, buffer (with the new data
    // else copy new data into existing buffer
    if (vertex_data.size() > capacity_)
    {
        handle_ = create_buffer(vertex_data);
        element_count_ = vertex_data.size();
        capacity_ = element_count_;
    }
    else
    {
        std::memcpy(
            handle_.contents,
            vertex_data.data(),
            vertex_data.size() * sizeof(VertexData));
        element_count_ = vertex_data.size();
    }
}

void MetalBuffer::write(const std::vector<std::uint32_t> &index_data)
{
    // if the new data is larger than existing buffer then allocate a new,
    // larger, buffer (with the new data
    // else copy new data into existing buffer
    if (index_data.size() > capacity_)
    {
        handle_ = create_buffer(index_data);
        element_count_ = index_data.size();
        capacity_ = element_count_;
    }
    else
    {
        std::memcpy(
            handle_.contents,
            index_data.data(),
            index_data.size() * sizeof(std::uint32_t));
        element_count_ = index_data.size();
    }
}

}
