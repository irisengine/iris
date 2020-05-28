#include "graphics/sprite.h"

#include "core/matrix4.h"
#include "core/vector3.h"
#include "graphics/material_factory.h"
#include "graphics/shape_factory.h"
#include "log/log.h"

namespace eng
{

Sprite::Sprite(
    const float x,
    const float y,
    const float width,
    const float height,
    const Vector3 &colour)
    : Sprite(x, y, width, height, colour, Texture::blank())
{ }

Sprite::Sprite(
    const float x,
    const float y,
    const float width,
    const float height,
    const Vector3 &colour,
    Texture &&tex)
    : mesh_(shape_factory::sprite(colour, std::move(tex))),
      position_(x, y, 0.0f),
      scale_(width, height, 1.0f),
      model_(),
      material_(material_factory::sprite()),
      wireframe_(false)
{
    model_ = Matrix4::make_translate(position_) * Matrix4::make_scale(scale_);

    LOG_ENGINE_INFO("entity", "constructed at: {} {}", position_, scale_);
}

void Sprite::set_position(const Vector3 &position)
{
    position_ = position;

    model_ = Matrix4::make_translate(position_)  * Matrix4::make_scale(scale_);
}

void Sprite::set_orientation(float orientation)
{
    model_ = Matrix4::make_translate(position_) * Matrix4::make_rotate_z(orientation) * Matrix4::make_scale(scale_);
}

Matrix4 Sprite::transform() const
{
    return model_;
}

const Mesh& Sprite::mesh() const
{
    return mesh_;
}

const Material& Sprite::material() const
{
    return *material_;
}

bool Sprite::should_render_wireframe() const
{
    return wireframe_;
}

void Sprite::set_wireframe(const bool wireframe)
{
    wireframe_ = wireframe;
}

}

