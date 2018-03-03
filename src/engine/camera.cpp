#include "camera.hpp"

#include <cmath>

#include "matrix4.cpp"
#include "vector3.hpp"

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
eng::vector3 create_direction(const float pitch, const float yaw) noexcept
{
    eng::vector3 direction;

    direction.x = std::cos(yaw) * std::cos(pitch);
    direction.y = std::sin(pitch);
    direction.z = std::sin(yaw) * std::cos(pitch);

    direction.normalise();

    return direction;
}

}

namespace eng
{

camera::camera()
    : position_(),
      direction_(0.0f, 0.0f, -1.0f),
      up_(0.0f, 1.0f, 0.0f),
      view_(),
      projection_(matrix4::make_projection(0.785398f, 1.0f, 0.1f, 1000.0f)),
      pitch_(0.0f),
      yaw_(-3.141592654f / 2.0f)
{
    direction_ = create_direction(pitch_, yaw_);
    view_ = matrix4::make_look_at(position_, position_ + direction_, up_);
}

void camera::translate(const vector3 &t) noexcept
{
    position_ += t;
    view_ = matrix4::make_look_at(position_, position_ + direction_, up_);
}

vector3 camera::position() const noexcept
{
    return position_;
}

vector3 camera::direction() const noexcept
{
    return direction_;
}

vector3 camera::right() const noexcept
{
    return vector3::normalise(vector3::cross(direction_, up_));
}

matrix4 camera::view() const noexcept
{
    return view_;
}

matrix4 camera::projection() const noexcept
{
    return projection_;
}

void camera::set_yaw(const float yaw) noexcept
{
    yaw_ = yaw;

    direction_ = create_direction(pitch_, yaw_);
    view_ = matrix4::make_look_at(position_, position_ + direction_, up_);
}

void camera::adjust_yaw(const float adjust) noexcept
{
    set_yaw(yaw_ + adjust);
}

void camera::set_pitch(const float pitch) noexcept
{
    pitch_ = pitch;

    direction_ = create_direction(pitch_, yaw_);
    view_ = matrix4::make_look_at(position_, position_ + direction_, up_);
}

void camera::adjust_pitch(const float adjust) noexcept
{
    set_pitch(pitch_ + adjust);
}

}

