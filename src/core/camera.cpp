#include "core/camera.h"

#include <cmath>

#include "core/matrix4.h"
#include "core/vector3.h"
#include "log/log.h"

namespace
{

// depth of renderable area
static constexpr auto render_depth = 800.0f;

/**
 * Helper method to create a direction vector from a pitch and yaw
 *
 * @param pitch
 *   pitch (in radians) of camera
 *
 * @param yaw
 *   yaw (in radians) of camera
 *
 * @returns
 *   A new direction vector for the camera
 */
eng::Vector3 create_direction(const float pitch, const float yaw)
{
    eng::Vector3 direction;

    direction.x = std::cos(yaw) * std::cos(pitch);
    direction.y = std::sin(pitch);
    direction.z = std::sin(yaw) * std::cos(pitch);

    direction.normalise();

    return direction;
}

}

namespace eng
{

Camera::Camera(float width, float height)
    : position_(0.0f, 0.0f, 100.0f),
      direction_(0.0f, 0.0f, -1.0f),
      up_(0.0f, 1.0f, 0.0f),
      view_(),
      projection_(Matrix4::make_projection(width, height, render_depth)),
      pitch_(0.0f),
      yaw_(-3.141592654f / 2.0f)
{
    direction_ = create_direction(pitch_, yaw_);
    view_ = Matrix4::make_look_at(position_, position_ + direction_, up_);

    LOG_ENGINE_INFO("camera", "constructed");
}

void Camera::translate(const Vector3 &translate)
{
    position_ += translate;
    view_ = Matrix4::make_look_at(position_, position_ + direction_, up_);
}

void Camera::set_view(const Matrix4 &view)
{
    view_ = view;
}

Vector3 Camera::position() const
{
    return position_;
}

Vector3 Camera::direction() const
{
    return direction_;
}

Vector3 Camera::right() const
{
    return Vector3::normalise(Vector3::cross(direction_, up_));
}

Matrix4 Camera::view() const
{
    return view_;
}

Matrix4 Camera::projection() const
{
    return projection_;
}

void Camera::set_yaw(const float yaw)
{
    yaw_ = yaw;

    direction_ = create_direction(pitch_, yaw_);
    view_ = Matrix4::make_look_at(position_, position_ + direction_, up_);
}

void Camera::adjust_yaw(const float adjust)
{
    set_yaw(yaw_ + adjust);
}

void Camera::set_pitch(const float pitch)
{
    pitch_ = pitch;

    direction_ = create_direction(pitch_, yaw_);
    view_ = Matrix4::make_look_at(position_, position_ + direction_, up_);
}

void Camera::adjust_pitch(const float adjust)
{
    set_pitch(pitch_ + adjust);
}

}
