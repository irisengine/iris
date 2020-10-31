#pragma once

#include <any>
#include <memory>

#include "graphics/buffer.h"
#include "graphics/vertex_attributes.h"

namespace iris
{

/**
 * Class encapsulating all data describing renderable vertices. This includes:
 *   - raw vertex data
 *   - vertex data attributes
 *   - indices (render order of vertices)
 */
class BufferDescriptor
{
  public:
    /**
     * Construct new BufferDescriptor.
     *
     * @param vertex_buffer
     *   Vertex data.
     *
     * @param index_buffer
     *   Index data.
     *
     * @param attributes
     *   Attributes for vertex.
     */
    BufferDescriptor(
        Buffer vertex_buffer,
        Buffer index_buffer,
        const VertexAttributes &attributes);

    ~BufferDescriptor();
    BufferDescriptor(BufferDescriptor &&);
    BufferDescriptor &operator=(BufferDescriptor &&);

    /**
     * Get a reference to the vertex data buffer.
     *
     * @returns
     *   Vertex data buffer.
     */
    const Buffer &vertex_buffer() const;

    /**
     * Get a reference to the index data buffer.
     *
     * @returns
     *   Index data buffer.
     */
    const Buffer &index_buffer() const;

    /**
     * Get native handle for buffer descriptor.
     *
     * @returns
     *   Buffer native handle.
     */
    std::any native_handle() const;

  private:
    /** Vertex data buffer. */
    Buffer vertex_buffer_;

    /** Index data buffer. */
    Buffer index_buffer_;

    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
