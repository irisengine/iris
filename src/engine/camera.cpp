#include "camera.hpp"

#include <cmath>

#include "vector3.hpp"

namespace eng
{

camera::camera()
    : position_(),
      view_()
{
    // calculate view matrix

    const auto pitch = 0.0f;
    const auto yaw = -3.141592654f / 2.0f;

    vector3 look_at{
        std::cos(yaw) * std::cos(pitch),
        std::sin(pitch),
        std::sin(yaw) * std::cos(pitch)
    };
    look_at.normalise();

    vector3 up{ 0.0f, 1.0f, 0.0f };

    auto s = vector3::cross(look_at, up);
    s.normalise();

    auto u = vector3::cross(s, look_at);
    u.normalise();

    // set view matrix
    view_[0] = s.x;
    view_[1] = s.y;
    view_[2] = s.z;
    view_[3] = 0.0f;
    view_[4] = u.x;
    view_[5] = u.y;
    view_[6] = u.z;
    view_[7] = 0.0f;
    view_[8] = look_at.x;
    view_[9] = look_at.y;
    view_[10] = look_at.z;
    view_[11] = 1.0f;
    view_[12] = 0.0f;
    view_[13] = 0.0f;
    view_[14] = 0.0f;
    view_[15] = 0.0f;
}

void camera::translate(const vector3 &t) noexcept
{
    position_ += t;
    view_ = view_ * matrix::make_translate(t);
}

vector3 camera::position() const noexcept
{
    return position_;
}

matrix camera::view() const noexcept
{
    return view_;
}

}

