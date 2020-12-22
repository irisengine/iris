#include "graphics/light.h"

#include "core/camera.h"
#include "core/matrix4.h"
#include "core/vector3.h"

namespace iris
{

Light::Light(const Vector3 &direction, bool cast_shadows)
    : direction_(direction)
    , shadow_camera_(CameraType::ORTHOGRAPHIC, 100.0f, 100.0f, 100.0f)
    , shadow_target_(1024u, 1024u)
    , cast_shadows_(cast_shadows)
{
    shadow_camera_.set_view(
        Matrix4::make_look_at(-direction_, {}, {0.0f, 1.0f, 0.0f}));
}

Vector3 Light::direction() const
{
    return direction_;
}

void Light::set_direction(const Vector3 &direction)
{
    direction_ = direction;
    shadow_camera_.set_view(
        Matrix4::make_look_at(-direction_, {}, {0.0f, 1.0f, 0.0f}));
}

bool Light::casts_shadows() const
{
    return cast_shadows_;
}

Camera &Light::shadow_camera()
{
    return shadow_camera_;
}

RenderTarget &Light::shadow_target()
{
    return shadow_target_;
}

}
