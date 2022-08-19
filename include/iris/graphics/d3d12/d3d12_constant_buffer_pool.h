////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <vector>

#include "graphics/d3d12/d3d12_constant_buffer.h"

namespace iris
{

/**
 * This class encapsulates a pool of D3D12ConstantBuffer objects as a fixed size buffer.
 */
class D3D12ConstantBufferPool
{
  public:
    /**
     * Construct a new D3D12ConstantBufferPool.
     *
     * @param frame
     *   The frame number the pool is for.
     */
    D3D12ConstantBufferPool(std::uint32_t frame);

    /**
     * Get the next D3D12ConstantBuffer in the circular buffer.
     *
     * @returns
     *   Next D3D12ConstantBuffer.
     */
    D3D12ConstantBuffer &next();

    /**
     * Reset the pool to the beginning, this will cause next() to recycle buffers.
     */
    void reset();

  private:
    /** Pool of D3D12ConstantBuffer objects. */
    std::vector<D3D12ConstantBuffer> buffers_;

    /** Index into pool of next free object. */
    std::size_t index_;
};

}
