#pragma once

#include <any>
#include <cstdint>
#include <vector>

#include "matrix4.hpp"
#include "quaternion.hpp"
#include "texture.hpp"
#include "vector3.hpp"
#include "vertex_data.hpp"

#if defined(GRAPHICS_API_OPENGL)
#include "gl/buffer.hpp"
#include "gl/mesh_implementation.hpp"
#elif defined(GRAPHICS_API_METAL)
#include "metal/buffer.hpp"
#include "metal/mesh_implementation.hpp"
#else
#error "no graphics api set"
#endif

namespace eng
{

/**
 * Class representing a renderable mesh.
 *
 * This class is designed to be de-coupled from the rendering system as much as
 * possible. An API specific implementation object is used internally to
 * achieve this.
 */
class mesh final
{
    public:

        /**
         * Construct a new mesh.
         *
         * @param vertices
         *   Collection of vertices to render.
         *
         * @param indices
         *   Collection of indices representing vertex draw order.
         *
         * @param tex
         *   The texture to render the mesh with.
         *
         * @param position
         *   Position in world space of mesh.
         *
         * @param scale
         *   Vector3 specifying amount to scale along each axis.
         */
        mesh(
            const std::vector<vertex_data> &vertices,
            const std::vector<std::uint32_t> &indices,
            texture &&tex);

        /** Default */
        ~mesh() = default;
        mesh(mesh&&) = default;
        mesh& operator=(mesh&&) = default;

        /** Disabled */
        mesh(const mesh&) = delete;
        mesh& operator=(const mesh&) = delete;

        /**
         * Get const reference to mesh indices.
         *
         * @returns
         *   Mesh vertices.
         */
        const std::vector<std::uint32_t>& indices() const noexcept;

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
         * Get a reference to the texture for this mesh.
         *
         * @returns
         *   Const reference to texture.
         */
        const texture& tex() const noexcept;

        /**
         * Get a native handle for the mesh. The type of this is dependant on
         * the current graphics API.
         *
         * @returns
         *   Graphics API specific handle.
         */
        std::any native_handle() const;

    private:

        /** Mesh index data. */
        std::vector<std::uint32_t> indices_;

        /** Texture to render mesh with. */
        texture texture_;

        /** Graphics API specific implementation. */
        std::unique_ptr<mesh_implementation> impl_;
};

}

