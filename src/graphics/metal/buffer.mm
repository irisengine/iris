#include "graphics/buffer.h"

#include <any>
#include <vector>

#import <Metal/Metal.h>

#include "graphics/buffer_type.h"
#include "graphics/vertex_data.h"
#include "core/macos/macos_ios_utility.h"

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
template<class T>
id<MTLBuffer> create_Buffer(const std::vector<T> &data)
{
    auto *device = iris::core::utility::metal_device();

    // create Buffer with data
    return [device newBufferWithBytes:static_cast<const void*>(data.data())
                               length:sizeof(T) * data.size()
                              options:MTLResourceOptionCPUCacheModeDefault];
}

}

namespace iris
{

/**
 * Struct containing implementation specific data.
 */
struct Buffer::implementation
{
    /** Simple constructor which takes a value for each member. */
    implementation(id<MTLBuffer> handle)
        : handle(handle)
    { }

    /** Metal handle for buffer. */
    id<MTLBuffer> handle;
};

Buffer::Buffer(
    const DataBuffer &data,
    const BufferType type,
    std::size_t element_count)
    : impl_(std::make_unique<implementation>(create_Buffer(data))),
      type_(type),
      element_count_(element_count),
      data_(data)
{ }

/** Default. */
Buffer::~Buffer() = default;
Buffer::Buffer(Buffer &&other) = default;
Buffer& Buffer::operator=(Buffer &&other) = default;

std::any Buffer::native_handle() const
{
    return std::any{ impl_->handle };
}

BufferType Buffer::type() const
{
    return type_;
}

std::size_t Buffer::element_count() const
{
    return element_count_;
}

const DataBuffer& Buffer::data() const
{
    return data_;
}

}


