#pragma once

#include <experimental/filesystem>
#include <vector>

#include "matrix4.hpp"
#include "mesh.hpp"
#include "quaternion.hpp"

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
         * @param position
         *   Position of mesh in world space.
         *
         * @param scale
         *   Scale of entity.
         */
        entity(
            const std::experimental::filesystem::path &path,
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
         * Translate entity.
         *
         * @param t
         *   Amount to transalte.
         */
        void translate(const vector3 &t) noexcept;

        /**
         * Rotate entity by the given quaternion.
         *
         * @param q
         *   Quaternion to rotate by.
         */
        void rotate(const quaternion &q) noexcept;

        /**
         * Set the model matrix for the entity.
         *
         * @param model
         *   New model matrix.
         */
        void set_model(const matrix4 &model) noexcept;

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

