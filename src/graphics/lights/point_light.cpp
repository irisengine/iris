////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/lights/point_light.h"

#include <array>
#include <cstring>

#include "core/vector3.h"
#include "graphics/lights/light_type.h"

namespace iris
{

PointLight::PointLight(const Vector3 &position)
    : PointLight(position, {1.0f, 1.0f, 1.0f, 1.0f})
{
}

PointLight::PointLight(const Vector3 &position, const Colour &colour)
    : position_(position)
    , colour_(colour)
    , attenuation_terms_()
{
    attenuation_terms_.constant = 0.0f;
    attenuation_terms_.linear = 1.0f;
    attenuation_terms_.quadratic = 0.0f;
}

LightType PointLight::type() const
{
    return LightType::POINT;
}

std::array<float, 4u> PointLight::colour_data() const
{
    std::array<float, 4u> light_data{};
    light_data.fill(0.0f);

    // sanity check we have enough space
    static_assert(light_data.size() * sizeof(decltype(light_data)::value_type) >= sizeof(colour_));

    // copy light data straight into buffer
    std::memcpy(light_data.data(), &colour_, sizeof(colour_));

    return light_data;
}

std::array<float, 4u> PointLight::world_space_data() const
{
    std::array<float, 4u> light_data{};
    light_data.fill(0.0f);

    // sanity check we have enough space
    static_assert(light_data.size() * sizeof(decltype(light_data)::value_type) >= sizeof(position_));

    // copy light data straight into buffer
    std::memcpy(light_data.data(), &position_, sizeof(position_));

    return light_data;
}

std::array<float, 3u> PointLight::attenuation_data() const
{
    std::array<float, 3u> light_data{};
    light_data.fill(0.0f);

    // sanity check we have enough space
    static_assert(light_data.size() * sizeof(decltype(light_data)::value_type) == sizeof(attenuation_terms_));

    // copy light data straight into buffer
    std::memcpy(light_data.data(), &attenuation_terms_, sizeof(attenuation_terms_));

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

Colour PointLight::colour() const
{
    return colour_;
}

void PointLight::set_colour(const Colour &colour)
{
    colour_ = colour;
}

float PointLight::attenuation_constant_term() const
{
    return attenuation_terms_.constant;
}

void PointLight::set_attenuation_constant_term(float constant)
{
    attenuation_terms_.constant = constant;
}

float PointLight::attenuation_linear_term() const
{
    return attenuation_terms_.linear;
}

void PointLight::set_attenuation_linear_term(float linear)
{
    attenuation_terms_.linear = linear;
}

float PointLight::attenuation_quadratic_term() const
{
    return attenuation_terms_.quadratic;
}

void PointLight::set_attenuation_quadratic_term(float quadratic)
{
    attenuation_terms_.quadratic = quadratic;
}

}
