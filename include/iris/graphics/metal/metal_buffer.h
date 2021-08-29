#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#import <Metal/Metal.h>

#include "graphics/vertex_data.h"

namespace iris
{

/**
 * This class encapsulates a metal buffer. A buffer can be created with either
 * vertex or index data.
 */
class MetalBuffer
{
  public:
    /**
     * Construct a new MetalBuffer with vertex data.
     *
     * @param vertex_data
     *   Vertex data to copy to buffer.
     */
    MetalBuffer(const std::vector<VertexData> &vertex_data);

    /**
     * Construct a new MetalBuffer with index data.
     *
     * @param vertex_data
     *   Index data to copy to buffer.
     */
    MetalBuffer(const std::vector<std::uint32_t> &index_data);

    MetalBuffer(const MetalBuffer &) = delete;
    MetalBuffer &operator=(const MetalBuffer &) = delete;

    /**
     * Get the metal handle to the buffer.
     *
     * @returns
     *   Metal handle.
     */
    id<MTLBuffer> handle() const;

    /**
     * Get the number of elements stored in the buffer.
     *
     * @returns
     *   Number of elements in buffer.
     */
    std::size_t element_count() const;

    /**
     * Write vertex data to the buffer.
     *
     * @param vertex_data
     *   New vertex data.
     */
    void write(const std::vector<VertexData> &vertex_data);

    /**
     * Write index data to the buffer.
     *
     * @param index_data
     *   New index data.
     */
    void write(const std::vector<std::uint32_t> &index_data);

  private:
    /** Metal handle for buffer. */
    id<MTLBuffer> handle_;

    /** Number of elements in buffer. */
    std::size_t element_count_;

    /** Maximum number of elements that can be stored in buffer. */
    std::size_t capacity_;
};

}
