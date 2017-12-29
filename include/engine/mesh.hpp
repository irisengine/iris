#pragma once

#include <cstdint>
#include <vector>

#include "gl/mesh_implementation.hpp"
#include "matrix.hpp"
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
         * @param colour
         *   Colour of vertices.
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
            const std::uint32_t colour,
            const vector3 &position,
            const vector3 &scale);

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
         * Translate the mesh.
         *
         * @param t
         *   Amount to translate.
         */
        void translate(const vector3 &t) noexcept;

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
         * Get colour.
         *
         * @returns
         *   Mesh colour.
         */
        std::uint32_t colour() const noexcept;

        /**
         * Get the model transformation matrix.
         *
         * @returns
         *   Model matrix.
         */
        matrix model() const noexcept;

    private:

        /** Mesh vertex data. */
        std::vector<vertex_data> vertices_;

        /** Mesh index data. */
        std::vector<std::uint32_t> indices_;

        /** Mesh colour. */
        std::uint32_t colour_;

        /** Model transformation matrix. */
        matrix model_;

        /** Graphics API specific implementation. */
        gl::mesh_implementation impl_;
};

}

