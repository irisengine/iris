#pragma once

#include <any>
#include <cstdint>
#include <vector>

#include "metal/buffer.hpp"
#include "vertex_data.hpp"

namespace eng
{

/**
 * This class is the metal implementation of mesh. This encapsulates all the
 * logic and data needed for metal rendering.
 */
class mesh_implementation final
{
    public:

        /**
         * Create a new mesh_implementation.
         *
         * @param vertices
         *   Collection of vertex data, will get transfered to graphics card.
         *
         * @param indices
         *   Collection of indices, representing the draw order of the vertices.
         */
        mesh_implementation(
            const std::vector<vertex_data> &vertices,
            const std::vector<std::uint32_t> &indices);

        /** Default */
        ~mesh_implementation() = default;
        mesh_implementation(mesh_implementation&&) = default;
        mesh_implementation& operator=(mesh_implementation&&) = default;

        /** Disabled */
        mesh_implementation(const mesh_implementation&) = delete;
        mesh_implementation& operator=(const mesh_implementation&) = delete;

        /**
         * Get a reference to the vertex buffer for this mesh.
         *
         * @returns
         *   Const reference to vertex buffer.
         */
        const buffer& vertex_buffer() const noexcept;

        /**
         * Get a reference to the index buffer for this mesh.
         *
         * @returns
         *   Const reference to index buffer.
         */
        const buffer& index_buffer() const noexcept;

        /**
         * Unused in metal.
         *
         * @returns
         *   Empty std::any.
         */
        std::any native_handle() const;

    private:

        /** The vertex data buffer. */
        std::unique_ptr<buffer> vertex_buffer_;

        /** The index data buffer. */
        std::unique_ptr<buffer> index_buffer_;
};

}


