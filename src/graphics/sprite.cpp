#include "graphics/sprite.h"

#include "core/camera_type.h"
#include "core/vector3.h"
#include "graphics/material_factory.h"
#include "graphics/shape_factory.h"
#include "graphics/texture.h"
#include "log/log.h"

namespace eng
{

Sprite::Sprite(
    real x,
    real y,
    real width,
    real height,
    const Vector3 &colour)
    : Sprite(x, y, width, height, colour, Texture::blank())
{ }

Sprite::Sprite(
    real x,
    real y,
    real width,
    real height,
    const Vector3 &colour,
    Texture &&tex)
    : RenderEntity(
        shape_factory::sprite(colour, std::move(tex)),
        { x, y, 0.0f },
        { },
        { width, height, 1.0f },
        material_factory::sprite(),
        false,
        CameraType::ORTHOGRAPHIC)
{
    LOG_ENGINE_INFO("sprite", "constructed at: {} {}", position_, scale_);
}

}

