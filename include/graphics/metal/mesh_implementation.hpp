#pragma once

#include <any>
#include <cstdint>
#include <vector>

#include "buffer.hpp"
#include "vertex_data.hpp"

namespace eng
{

/**
 * This class is the metal implementation of mesh. This encapsulates all the
 * logic and data needed for metal rendering.
 */
class Mesh_implementation
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

        /** Disabled */
        mesh_implementation(const mesh_implementation&) = delete;
        mesh_implementation& operator=(const mesh_implementation&) = delete;

        /**
         * Get a reference to the vertex Buffer for this mesh.
         *
         * @returns
         *   Const reference to vertex buffer.
         */
        const Buffer& vertex_Buffer() const;

        /**
         * Get a reference to the index Buffer for this mesh.
         *
         * @returns
         *   Const reference to index buffer.
         */
        const Buffer& index_Buffer() const;

        /**
         * Unused in metal.
         *
         * @returns
         *   Empty std::any.
         */
        std::any native_handle() const;

    private:

        /** The vertex data buffer. */
        std::unique_ptr<Buffer> vertex_buffer_;

        /** The index data buffer. */
        std::unique_ptr<Buffer> index_buffer_;
};

}


