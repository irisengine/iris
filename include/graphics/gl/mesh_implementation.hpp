#pragma once

#include <cstdint>
#include <vector>

#include "gl/buffer.hpp"
#include "gl/vertex_state.hpp"
#include "vector3.hpp"
#include "vertex_data.hpp"

namespace eng
{

/**
 * This class is the opengl implementation of mesh. This encapsulates all the
 * logic and data needed for opengl rendering.
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
        mesh_implementation(const mesh_implementation&) = delete;;
        mesh_implementation& operator=(const mesh_implementation&) = delete;

        /**
         * Performs all opengl actions required to be able to render the mesh.
         */
        void bind() const;

        /**
         * Performs all opengl actions after rendering is done.
         */
        void unbind() const;

    private:

        /**
         * The vertex array object. */
        vertex_state vao_;

        /** The vertex buffer object. */
        buffer vbo_;

        /** The element buffer object. */
        buffer ebo_;
};

}

