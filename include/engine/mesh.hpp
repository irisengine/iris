#pragma once

#include <cstdint>
#include <vector>

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
         */
        mesh(const std::vector<float> &vertices, const std::uint32_t colour);

        /** Default */
        ~mesh() = default;
        mesh(const mesh&) = default;
        mesh& operator=(const mesh&) = default;
        mesh(mesh&&) = default;
        mesh& operator=(mesh&&) = default;

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

    private:

        /** Mesh vertex data. */
        std::vector<float> vertices_;

        /** Mesh colour. */
        std::uint32_t colour_;
};

}

