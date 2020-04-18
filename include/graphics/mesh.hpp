#pragma once

#include <any>
#include <cstdint>
#include <memory>
#include <vector>

#include "core/matrix4.hpp"
#include "core/quaternion.hpp"
#include "core/vector3.hpp"
#include "graphics/buffer.hpp"
#include "graphics/texture.hpp"
#include "graphics/vertex_data.hpp"

namespace eng
{

/**
 * Class representing a renderable mesh.
 *
 * This class is designed to be de-coupled from the rendering system as much as
 * possible. An API specific implementation object is used internally to
 * achieve this.
 */
class Mesh
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
         *   The Texture to render the Mesh with.
         *
         * @param position
         *   Position in world space of mesh.
         *
         * @param scale
         *   Vector3 specifying amount to scale along each axis.
         */
        Mesh(
            const std::vector<vertex_data> &vertices,
            const std::vector<std::uint32_t> &indices,
            Texture &&tex);

        /** Declared in mm/cpp file as implementation is an incomplete file. */
        ~Mesh();
        Mesh(Mesh&&);
        Mesh& operator=(Mesh&&);

        /**
         * Get const reference to Mesh indices.
         *
         * @returns
         *   Mesh vertices.
         */
        const std::vector<std::uint32_t>& indices() const;

        /**
         * Get a reference to the vertex Buffer for this mesh.
         *
         * @returns
         *   Const reference to vertex buffer.
         */
        const Buffer& vertex_buffer() const;

        /**
         * Get a reference to the index Buffer for this mesh.
         *
         * @returns
         *   Const reference to index buffer.
         */
        const Buffer& index_buffer() const;

        /**
         * Get a reference to the Texture for this mesh.
         *
         * @returns
         *   Const reference to texture.
         */
        const Texture& texture() const;

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

        /** Texture to render Mesh with. */
        Texture texture_;

        /** Buffer for vertex data. */
        std::unique_ptr<Buffer> vertex_buffer_;

        /** Buffer for vertex indices. */
        std::unique_ptr<Buffer> index_buffer_;

        /** Graphics API specific implementation. */
        struct implementation;
        std::unique_ptr<implementation> impl_;
};

}

