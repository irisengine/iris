#pragma once

#include <cstdint>
#include <vector>

#include "gl/mesh_implementation.hpp"
#include "matrix4.hpp"
#include "quaternion.hpp"
#include "texture.hpp"
#include "vector3.hpp"
#include "vertex_data.hpp"

namespace eng
{

// forward declaration
struct mesh_implementation;

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
         * Perform all actions needed to render.
         */
        void bind() const;

        /**
         * Perform all actions needed after rendering.
         */
        void unbind() const;

        /**
         * Get const reference to mesh vertices.
         *
         * @returns
         *   Mesh vertices.
         */
        const std::vector<vertex_data>& vertices() const noexcept;

        /**
         * Get const reference to mesh indices.
         *
         * @returns
         *   Mesh vertices.
         */
        const std::vector<std::uint32_t>& indices() const noexcept;

        /**
         * Get the model transformation matrix4.
         *
         * @returns
         *   Model matrix4.
         */
        matrix4 model() const noexcept;

        void set_model(const matrix4 &model) noexcept;

    private:

        /** Mesh vertex data. */
        std::vector<vertex_data> vertices_;

        /** Mesh index data. */
        std::vector<std::uint32_t> indices_;

        /** Texture to render mesh with. */
        texture texture_;

        /** Graphics API specific implementation. */
        gl::mesh_implementation impl_;
};

}

