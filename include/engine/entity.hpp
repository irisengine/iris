#pragma once

#include <experimental/filesystem>
#include <vector>

#include "mesh.hpp"

namespace eng
{

/**
 * Class represents a renderable entity that can be added to a scene. An entity
 * comprises of one or more meshes that should be rendered individually.
 */
class entity final
{
    public:

        /**
         * Create a new entity.
         *
         * @param path
         *   Path to model file to load.
         *
         * @param colour
         *   Colour of meshes.
         *
         * @param position
         *   Position of mesh in world space.
         *
         * @param scale
         *   Scale of entity.
         */
        entity(
            const std::experimental::filesystem::path &path,
            const std::uint32_t colour,
            const vector3 &position,
            const vector3 &scale);

        /** Default */
        ~entity() = default;
        entity(entity&&) = default;
        entity& operator=(entity&&) = default;

        /** Disabled */
        entity(const entity&) = delete;
        entity& operator=(const entity&) = delete;

        /**
         * Get a const reference to the meshes that make up the entity.
         *
         * @returns
         *   Const reference to meshes.
         */
        const std::vector<mesh>& meshes() const noexcept;

    private:

        /** Collection of meshes. */
        std::vector<mesh> meshes_;
};

}

