#pragma once

#include "core/camera_type.h"
#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/vector3.h"

namespace iris
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
     * @param
     *   Type of camera.
     *
     * @param width
     *   Width of window.
     *
     * @param height
     *   Height of window.
     * 
     * @param depth
     *   Depth of projection.
     */
    Camera(CameraType type, float width, float height, float depth = 1000.0f);

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
     * Get orientation of camera.
     *
     * @returns
     *   Camera orientation.
     */
    Quaternion orientation() const;

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
     * Get camera yaw.
     *
     * @returns
     *   Camera yaw.
     */
    float yaw() const;

    /**
     * Set the yaw of the camera.
     *
     * @param yaw
     *   New camera yaw.
     */
    void set_yaw(float yaw);

    /**
     * Adjust the camera yaw by the supplied value.
     *
     * @param adjust
     *   Amount to adjust yaw by.
     */
    void adjust_yaw(float adjust);

    /**
     * Get camera pitch.
     *
     * @returns
     *   Camera pitch.
     */
    float pitch() const;

    /**
     * Set the pitch of the camera.
     *
     * @param pitch
     *   New camera pitch.
     */
    void set_pitch(float pitch);

    /**
     * Adjust the camera pitch by the supplied value.
     *
     * @param adjust
     *   Amount to adjust pitch by.
     */
    void adjust_pitch(float adjust);

    /**
     * Set world position of camera.
     *
     * @param position
     *   New position.
     */
    void set_position(const Vector3 &position);

    /**
     * Get type of camera.
     *
     * @returns
     *   Camera type.
     */
    CameraType type() const;

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

    /** Type of camera. */
    CameraType type_;
};

}
