#pragma once

#include "matrix.hpp"
#include "vector3.hpp"

namespace eng
{

/**
 * Class representing the camera through which a scene is rendered.
 */
class camera final
{
    public:

        /**
         * Create a new camera, positioned at the origin.
         */
        camera();

        /** Default */
        ~camera() = default;
        camera(const camera&) = default;
        camera& operator=(const camera&) = default;
        camera(camera&&) = default;
        camera& operator=(camera&&) = default;

        /**
         * Translate the camera.
         *
         * @param t
         *   Amount to translate.
         */
        void translate(const vector3 &t) noexcept;

        /**
         * Get position of camera.
         *
         * @returns
         *   Camera position in world space.
         */
        vector3 position() const noexcept;

        /**
         * Get the view matrix.
         *
         * @returns
         *   View matrix.
         */
        matrix view() const noexcept;

   private:

        /** Camera position in world space. */
        vector3 position_;

        /** View matrix for the camera. */
        matrix view_;
};

}

