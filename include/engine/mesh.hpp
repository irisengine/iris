#pragma once

#include <cstdint>
#include <vector>

#include "matrix.hpp"
#include "vector3.hpp"

namespace eng
{

/**
 * Class representing a renderable mesh.
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
         * @param colour
         *   Colour of vertices.
         *
         * @param position
         *   Position in world space of mesh.
         *
         * @param scale
         *   Scale of mesh.
         */
        mesh(
            const std::vector<float> &vertices,
            const std::uint32_t colour,
            const vector3 &position,
            const float scale);

        /** Default */
        ~mesh() = default;
        mesh(const mesh&) = default;
        mesh& operator=(const mesh&) = default;
        mesh(mesh&&) = default;
        mesh& operator=(mesh&&) = default;

        /**
         * Translate the mesh.
         *
         * @param t
         *   Amount to translate.
         */
        void translate(const vector3 &t) noexcept;

        /**
         * Get mesh vertices.
         *
         * @returns
         *   Mesh vertices.
         */
        const std::vector<float> vertices() const noexcept;

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
        std::vector<float> vertices_;

        /** Mesh colour. */
        std::uint32_t colour_;

        /** Model transformation matrix. */
        matrix model_;
};

}

