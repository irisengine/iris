#include "buffer.hpp"

#include <any>
#include <vector>

#import <Metal/Metal.h>

#include "buffer_type.hpp"
#include "vertex_data.hpp"

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
id<MTLBuffer> create_buffer(const std::vector<T> &data)
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

/**
 * Struct containing implementation specific data.
 */
struct buffer::implementation
{
    /** Simple constructor which takes a value for each member. */
    implementation(id<MTLBuffer> handle)
        : handle(handle)
    { }

    /** Metal handle for buffer. */
    id<MTLBuffer> handle;
};

buffer::buffer(const std::vector<float> &data, const buffer_type type)
    : impl_(std::make_unique<implementation>(create_buffer(data))),
      type_(type)
{ }

buffer::buffer(const std::vector<std::uint32_t> &data, const buffer_type type)
    : impl_(std::make_unique<implementation>(create_buffer(data))),
      type_(type)
{ }

buffer::buffer(const std::vector<vertex_data> &data, const buffer_type type)
    : impl_(std::make_unique<implementation>(create_buffer(data))),
      type_(type)
{ }

/** Default. */
buffer::~buffer() = default;
buffer::buffer(buffer &&other) = default;
buffer& buffer::operator=(buffer &&other) = default;

std::any buffer::native_handle() const
{
    return std::any{ impl_->handle };
}

buffer_type buffer::type() const
{
    return type_;
}

}


