#include "graphics/lights/ambient_light.h"

#include <array>
#include <cstring>

#include "core/colour.h"
#include "graphics/lights/light.h"
#include "graphics/lights/light_type.h"

namespace iris
{

AmbientLight::AmbientLight(const Colour &colour)
    : colour_(colour)
{
}

LightType AmbientLight::type() const
{
    return LightType::AMBIENT;
}

std::array<float, 4u> AmbientLight::colour_data() const
{
    std::array<float, 4u> light_data{};
    light_data.fill(0.0f);

    // sanity check we have enough space
    static_assert(
        light_data.size() * sizeof(decltype(light_data)::value_type) >=
        sizeof(colour_));

    // copy light data straight into buffer
    std::memcpy(light_data.data(), &colour_, sizeof(colour_));

    return light_data;
}

std::array<float, 4u> AmbientLight::world_space_data() const
{
    std::array<float, 4u> light_data{};
    light_data.fill(0.0f);

    return light_data;
}

std::array<float, 3u> AmbientLight::attenuation_data() const
{
    std::array<float, 3u> light_data{};
    light_data.fill(0.0f);

    return light_data;
}

Colour AmbientLight::colour() const
{
    return colour_;
}

void AmbientLight::set_colour(const Colour &colour)
{
    colour_ = colour;
}

}
