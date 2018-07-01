#include "metal/buffer.hpp"

#include <any>
#include <vector>

#import <Metal/Metal.h>

namespace
{

/**
 * Helper function to create a metal buffer from a collection of objects.
 *
 * @param data
 *   Data to store on buffer.
 *
 * @returns
 *   Handle to metal buffer.
 */
template<class T>
id<MTLBuffer> create_metal_buffer(const std::vector<T> &data)
{
    // get metal device handle
    static const auto *device =
        ::CGDirectDisplayCopyCurrentMetalDevice(::CGMainDisplayID());

    // create buffer with data
    return [device newBufferWithBytes:static_cast<const void*>(data.data())
                               length:sizeof(T) * data.size()
                              options:MTLResourceOptionCPUCacheModeDefault];
}

}

namespace eng
{

buffer::buffer(const std::vector<float> &data)
    : buffer_(create_metal_buffer(data))
{ }

buffer::buffer(const std::vector<std::uint32_t> &data)
    : buffer_(create_metal_buffer(data))
{ }

std::any buffer::native_handle() const
{
    return buffer_;
}

}


