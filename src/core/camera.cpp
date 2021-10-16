////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/camera.h"

#include <cmath>

#include "core/camera_type.h"
#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/vector3.h"
#include "log/log.h"

namespace
{

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
iris::Vector3 create_direction(float pitch, float yaw)
{
    iris::Vector3 direction;

    direction.x = std::cos(yaw) * std::cos(pitch);
    direction.y = std::sin(pitch);
    direction.z = std::sin(yaw) * std::cos(pitch);

    direction.normalise();

    return direction;
}

}

namespace iris
{

Camera::Camera(CameraType type, std::uint32_t width, std::uint32_t height, std::uint32_t depth)
    : position_(0.0f, 0.0f, 100.0f)
    , direction_(0.0f, 0.0f, -1.0f)
    , up_(0.0f, 1.0f, 0.0f)
    , view_()
    , projection_()
    , pitch_(0.0f)
    , yaw_(-3.141592654f / 2.0f)
    , type_(type)
{
    const auto width_f = static_cast<float>(width);
    const auto height_f = static_cast<float>(height);
    const auto depth_f = static_cast<float>(depth);

    switch (type_)
    {
        case CameraType::PERSPECTIVE:
            projection_ = Matrix4::make_perspective_projection(0.785398f, width_f, height_f, 0.1f, depth_f);
            break;
        case CameraType::ORTHOGRAPHIC:
            projection_ = Matrix4::make_orthographic_projection(width_f, height_f, depth_f);
            break;
    }

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

Quaternion Camera::orientation() const
{
    return {yaw_, pitch_, 0.0f};
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

float Camera::yaw() const
{
    return yaw_;
}

void Camera::set_yaw(float yaw)
{
    yaw_ = yaw;

    direction_ = create_direction(pitch_, yaw_);
    view_ = Matrix4::make_look_at(position_, position_ + direction_, up_);
}

void Camera::adjust_yaw(float adjust)
{
    set_yaw(yaw_ + adjust);
}

float Camera::pitch() const
{
    return pitch_;
}

void Camera::set_pitch(float pitch)
{
    pitch_ = pitch;

    direction_ = create_direction(pitch_, yaw_);
    view_ = Matrix4::make_look_at(position_, position_ + direction_, up_);
}

void Camera::set_position(const Vector3 &position)
{
    position_ = position;
    view_ = Matrix4::make_look_at(position_, position_ + direction_, up_);
}

void Camera::adjust_pitch(float adjust)
{
    set_pitch(pitch_ + adjust);
}

CameraType Camera::type() const
{
    return type_;
}

}
