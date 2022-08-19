////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_constant_buffer_pool.h"

#include <cstdint>

#include "core/error_handling.h"

namespace iris
{

D3D12ConstantBufferPool::D3D12ConstantBufferPool(std::uint32_t frame)
    : buffers_()
    , index_(0u)
{
    for (auto i = 0u; i < 400; ++i)
    {
        buffers_.emplace_back(frame, 7168u);
    }
}

D3D12ConstantBuffer &D3D12ConstantBufferPool::next()
{
    ensure(index_ < buffers_.size(), "pool drained");

    return buffers_[index_++];
}

void D3D12ConstantBufferPool::reset()
{
    index_ = 0u;
}

}
