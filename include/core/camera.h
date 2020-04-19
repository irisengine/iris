#pragma once

#include "matrix4.h"
#include "vector3.h"

namespace eng
{

/**
 * Class representing the camera through which a scene is rendered.
 */
class Camera
{
    public:

        /**
         * Create a new camera, positioned at the origin.
         *
         * @param width
         *   Width of window.
         *
         * @param height
         *   Height of window.
         */
        Camera(float width, float height);

        /**
         * Translate the camera.
         *
         * @param translate
         *   Amount to translate.
         */
        void translate(const Vector3 &translat);

        /**
         * Set the view matrix for the camera.
         *
         * @param view
         *   New view matrix.
         */
        void set_view(const Matrix4 &view);

        /**
         * Get position of camera.
         *
         * @returns
         *   Camera position in world space.
         */
        Vector3 position() const;

        /**
         * Get direction camera is facing.
         *
         * @returns
         *   Camera direction.
         */
        Vector3 direction() const;

        /**
         * Get the right vector of the camera.
         *
         * @returns
         *   Camera right vector.
         */
        Vector3 right() const;

        /**
         * Get the view matrix4.
         *
         * @returns
         *   View matrix4.
         */
        Matrix4 view() const;

        /**
         * Get the projection matrix4.
         *
         * @returns
         *   Projection matrix4.
         */
        Matrix4 projection() const;

        /**
         * Set the yaw of the camera.
         *
         * @param yaw
         *   New camera yaw.
         */
        void set_yaw(const float yaw);

        /**
         * Adjust the camera yaw by the supplied value.
         *
         * @param adjust
         *   Amount to adjust yaw by.
         */
        void adjust_yaw(const float adjust);

        /**
         * Set the pitch of the camera.
         *
         * @param pitch
         *   New camera pitch.
         */
        void set_pitch(const float pitch);

        /**
         * Adjust the camera pitch by the supplied value.
         *
         * @param adjust
         *   Amount to adjust pitch by.
         */
        void adjust_pitch(const float adjust);

   private:

        /** Camera position in world space. */
        Vector3 position_;

        /** Direction camera is facing. */
        Vector3 direction_;

        /** Camera up vector. */
        Vector3 up_;

        /** View Matrix4 for the camera. */
        Matrix4 view_;

        /** Projection Matrix4 for the camera. */
        Matrix4 projection_;

        /** Pitch of camera. */
        float pitch_;

        /** Yaw of camera. */
        float yaw_;
};

}

