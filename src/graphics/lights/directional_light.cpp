////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/lights/directional_light.h"

#include <array>
#include <cstring>

#include "core/camera.h"
#include "core/matrix4.h"
#include "core/vector3.h"
#include "graphics/lights/light_type.h"

namespace iris
{

DirectionalLight::DirectionalLight(const Vector3 &direction, bool cast_shadows)
    : direction_(direction)
    , shadow_camera_(CameraType::ORTHOGRAPHIC, 100u, 100u, 1000u)
    , cast_shadows_(cast_shadows)
{
    shadow_camera_.set_view(Matrix4::make_look_at(-direction_, {}, {0.0f, 1.0f, 0.0f}));
}

LightType DirectionalLight::type() const
{
    return LightType::DIRECTIONAL;
}

std::array<float, 4u> DirectionalLight::colour_data() const
{
    std::array<float, 4u> light_data{};
    light_data.fill(1.0f);

    return light_data;
}

std::array<float, 4u> DirectionalLight::world_space_data() const
{
    std::array<float, 4u> light_data{};
    light_data.fill(0.0f);

    static_assert(light_data.size() * sizeof(decltype(light_data)::value_type) >= sizeof(direction_));

    std::memcpy(light_data.data(), &direction_, sizeof(direction_));

    return light_data;
}

std::array<float, 3u> DirectionalLight::attenuation_data() const
{
    std::array<float, 3u> light_data{};
    light_data.fill(1.0f);

    return light_data;
}

Vector3 DirectionalLight::direction() const
{
    return direction_;
}

void DirectionalLight::set_direction(const Vector3 &direction)
{
    direction_ = direction;
    shadow_camera_.set_view(Matrix4::make_look_at(-direction_, {}, {0.0f, 1.0f, 0.0f}));
}

bool DirectionalLight::casts_shadows() const
{
    return cast_shadows_;
}

const Camera &DirectionalLight::shadow_camera() const
{
    return shadow_camera_;
}

}
