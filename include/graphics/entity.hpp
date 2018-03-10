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
         * @param orientation
         *   Orientation of entity.
         *
         * @param scale
         *   Scale of entity.
         */
        entity(
            const std::experimental::filesystem::path &path,
            const vector3 &position,
            const quaternion &orientation,
            const vector3 &scale);

        /** Default */
        ~entity() = default;
        entity(entity&&) = default;
        entity& operator=(entity&&) = default;

        /** Disabled */
        entity(const entity&) = delete;
        entity& operator=(const entity&) = delete;

        /**
         * Set the position of the entity.
         *
         * @param position
         *   New position.
         */
        void set_position(const vector3 &position) noexcept;

        /**
         * Set the orientation of the matrix.
         *
         * @param orientation
         *   New oritentation.
         */
        void set_orientation(const quaternion &orientation) noexcept;

        /**
         * Get the transformation matrix of the entity.
         *
         * @returns
         *   Transformation matrix.
         */
        matrix4 transform() const noexcept;

        /**
         * Get a const reference to the meshes that make up the entity.
         *
         * @returns
         *   Const reference to meshes.
         */
        const std::vector<mesh>& meshes() const noexcept;

        /**
         * Returns whether the object should be rendered as a wireframe.
         *
         * @returns
         *   True if should be rendered as a wireframe, false otherwise.
         */
        bool should_render_wireframe() const noexcept;

        /**
         * Sets whether the object should be rendered as a wireframe.
         *
         * @param wrireframe
         *   True if should be rendered as a wireframe, false otherwise.
         */
        void set_wireframe(const bool wireframe) noexcept;

    private:

        /** Collection of meshes. */
        std::vector<mesh> meshes_;

        /** The position of the entity. */
        vector3 position_;

        /** The orientation of the entity. */
        quaternion orientation_;

        /** The scale of the entity. */
        vector3 scale_;

        /** Model transformation matrix4. */
        matrix4 model_;

        /** Whether the object should be rendered as a wireframe. */
        bool wireframe_;
};

}

