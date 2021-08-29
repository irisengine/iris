#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <wrl.h>

#include "directx/d3d12.h"

#include "graphics/vertex_data.h"

namespace iris
{

/**
 * This class encapsulates a d3d12 buffer. A buffer can be created with either
 * vertex or index data.
 *
 * Note that this class provides access to d3d12 views to the data stored. These
 * are accessed via either vertex_view() or index_view(), however only one of
 * these calls will be valid and that is based in which constructor was used.
 * This is an internal class and as such the engine knows how to correctly call
 * the required view. This class does *not* perform any checks on the view
 * class. Calling the incorrect view call is undefined.
 */
class D3D12Buffer
{
  public:
    /**
     * Construct a new D3D12Buffer with vertex data.
     *
     * @param vertex_data
     *   Vertex data to copy to buffer.
     */
    D3D12Buffer(const std::vector<VertexData> &vertex_data);

    /**
     * Construct a new D3D12Buffer with index data.
     *
     * @param vertex_data
     *   Index data to copy to buffer.
     */
    D3D12Buffer(const std::vector<std::uint32_t> &index_data);

    D3D12Buffer(const D3D12Buffer &) = delete;
    D3D12Buffer &operator=(const D3D12Buffer &) = delete;

    /**
     * Get the number of elements stored in the buffer.
     *
     * @returns
     *   Number of elements in buffer.
     */
    std::size_t element_count() const;

    /**
     * Get the native view to the vertex data. Only valid if the vertex data
     * constructor was used.
     *
     * @returns
     *   D3D12 view to vertex data.
     */
    D3D12_VERTEX_BUFFER_VIEW vertex_view() const;

    /**
     * Get the native view to the index data. Only valid if the index data
     * constructor was used.
     *
     * @returns
     *   D3D12 view to index data.
     */
    D3D12_INDEX_BUFFER_VIEW index_view() const;

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
    /** D3D12 handle to buffer. */
    ::Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

    /** View to vertex data, only valid for vertex data constructor. */
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view_;

    /** View to index data, only valid for index data constructor. */
    D3D12_INDEX_BUFFER_VIEW index_buffer_view_;

    /** Number of elements in buffer. */
    std::size_t element_count_;

    /** Maximum number of elements that can be stored in buffer. */
    std::size_t capacity_;

    /** Pointer to mapped memory where new buffer data can be written. */
    std::byte *mapped_memory_;
};

}
