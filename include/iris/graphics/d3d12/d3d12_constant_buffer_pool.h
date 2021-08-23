#pragma once

#include <cstdint>
#include <vector>

#include "graphics/d3d12/d3d12_constant_buffer.h"

namespace iris
{

/**
 * This class encapsulates a pool of D3D12ConstantBuffer objects as a fixed size
 * circular buffer.
 */
class D3D12ConstantBufferPool
{
  public:
    /**
     * Construct a new D3D12ConstantBufferPool.
     */
    D3D12ConstantBufferPool();

    /**
     * Get the next D3D12ConstantBuffer in the circular buffer.
     *
     * @returns
     *   Next D3D12ConstantBuffer.
     */
    D3D12ConstantBuffer &next();

  private:
    /** Pool of D3D12ConstantBuffer objects. */
    std::vector<D3D12ConstantBuffer> buffers_;

    /** Index into pool of next free object. */
    std::size_t index_;
};

}
