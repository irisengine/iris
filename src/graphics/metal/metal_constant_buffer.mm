////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/metal/metal_constant_buffer.h"

#include <cstddef>

#import <Metal/Metal.h>

#include "core/macos/macos_ios_utility.h"

namespace iris
{

MetalConstantBuffer::MetalConstantBuffer(std::size_t capacity)
    : buffer_(nullptr)
    , capacity_(capacity)
{
    auto *device = iris::core::utility::metal_device();

    // create buffer with data
    buffer_ = [device newBufferWithLength:capacity_
                                  options:MTLResourceOptionCPUCacheModeDefault];
}

id<MTLBuffer> MetalConstantBuffer::handle() const
{
    return buffer_;
}

}
