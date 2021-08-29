#include "graphics/lights/point_light.h"

#include <array>
#include <cstring>

#include "core/vector3.h"
#include "graphics/lights/light_type.h"

namespace iris
{

PointLight::PointLight(const Vector3 &position)
    : position_(position)
{
}

LightType PointLight::type() const
{
    return LightType::POINT;
}

std::array<float, 4u> PointLight::data() const
{
    std::array<float, 4u> light_data{};
    light_data.fill(0.0f);

    // sanity check we have enough space
    static_assert(
        light_data.size() * sizeof(decltype(light_data)::value_type) >=
        sizeof(position_));

    // copy light data straight into buffer
    std::memcpy(light_data.data(), &position_, sizeof(position_));

    return light_data;
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
