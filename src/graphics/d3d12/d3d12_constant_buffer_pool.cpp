#include "graphics/d3d12/d3d12_constant_buffer_pool.h"

namespace iris
{

D3D12ConstantBufferPool::D3D12ConstantBufferPool()
    : buffers_()
    , index_(0u)
{
    for (auto i = 0u; i < 3; ++i)
    {
        buffers_.emplace_back(7168u);
    }
}

D3D12ConstantBuffer &D3D12ConstantBufferPool::next()
{
    return buffers_[index_++ % buffers_.size()];
}

}
