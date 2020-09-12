#include "graphics/buffer.h"

#include <any>
#include <vector>

#import <Metal/Metal.h>

#include "graphics/buffer_type.h"
#include "graphics/vertex_data.h"
#include "platform/macos/macos_ios_utility.h"

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
    auto *device = iris::platform::utility::metal_device();

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

Buffer::Buffer(const std::vector<float> &data, const BufferType type)
    : impl_(std::make_unique<implementation>(create_Buffer(data))),
      type_(type)
{ }

Buffer::Buffer(const std::vector<std::uint32_t> &data, const BufferType type)
    : impl_(std::make_unique<implementation>(create_Buffer(data))),
      type_(type)
{ }

Buffer::Buffer(const std::vector<vertex_data> &data, const BufferType type)
    : impl_(std::make_unique<implementation>(create_Buffer(data))),
      type_(type)
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

}


