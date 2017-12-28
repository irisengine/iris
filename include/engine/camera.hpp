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
         * Get direction camera is facing.
         *
         * @returns
         *   Camera direction.
         */
        vector3 direction() const noexcept;

        /**
         * Get the right vector of the camera.
         *
         * @returns
         *   Camera right vector.
         */
        vector3 right() const noexcept;

        /**
         * Get the view matrix.
         *
         * @returns
         *   View matrix.
         */
        matrix view() const noexcept;

        /**
         * Get the projection matrix.
         *
         * @returns
         *   Projection matrix.
         */
        matrix projection() const noexcept;

        /**
         * Set the yaw of the camera.
         *
         * @param yaw
         *   New camera yaw.
         */
        void set_yaw(const float yaw) noexcept;

        /**
         * Adjust the camera yaw by the supplied value.
         *
         * @param adjust
         *   Amount to adjust yaw by.
         */
        void adjust_yaw(const float adjust) noexcept;

        /**
         * Set the pitch of the camera.
         *
         * @param pitch
         *   New camera pitch.
         */
        void set_pitch(const float pitch) noexcept;

        /**
         * Adjust the camera pitch by the supplied value.
         *
         * @param adjust
         *   Amount to adjust pitch by.
         */
        void adjust_pitch(const float adjust) noexcept;

   private:

        /** Camera position in world space. */
        vector3 position_;

        /** Direction camera is facing. */
        vector3 direction_;

        /** Camera up vector. */
        vector3 up_;

        /** View matrix for the camera. */
        matrix view_;

        /** Projection matrix for the camera. */
        matrix projection_;

        /** Pitch of camera. */
        float pitch_;

        /** Yaw of camera. */
        float yaw_;
};

}

