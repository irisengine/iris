#include "graphics/lights/point_light.h"

#include "core/vector3.h"

namespace iris
{

PointLight::PointLight(const Vector3 &position)
    : position_(position)
{
}

Vector3 PointLight::position() const
{
    return position_;
}

void PointLight::set_position(const Vector3 &position)
{
    position_ = position;
}

}
