#include "plane.hpp"

#include "rigid_body_shape.hpp"
#include "rigid_body.hpp"
#include "vector3.hpp"

namespace eng
{

plane::plane(
    const vector3 &normal,
    const float offset)
    : rigid_body({ }, 1.0f, { }, rigid_body_shape::PLANE, true),
      normal_(normal),
      offset_(offset)
{ }

vector3 plane::normal() const noexcept
{
    return normal_;
}

float plane::offset() const noexcept
{
    return offset_;
}

}

