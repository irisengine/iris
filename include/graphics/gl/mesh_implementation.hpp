#pragma once

#include <any>
#include <cstdint>
#include <vector>

#include "core/vector3.hpp"
#include "graphics/buffer.hpp"
#include "graphics/gl/vertex_state.hpp"
#include "graphics/vertex_data.hpp"

namespace eng
{

/**
 * This class is the opengl implementation of mesh. This encapsulates all the
 * logic and data needed for opengl rendering.
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
         * Get a native handle for the mesh. The type of this is dependant on
         * the current graphics API.
         *
         * @returns
         *   Graphics API specific handle.
         */
        std::any native_handle() const;

    private:

        /** The vertex array object. */
        vertex_state vao_;

        /** The vertex Buffer object. */
        Buffer vbo_;

        /** The element Buffer object. */
        Buffer ebo_;
};

}

