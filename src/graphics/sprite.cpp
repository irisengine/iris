#include "sprite.hpp"

#include "entity.hpp"
#include "material_factory.hpp"
#include "shape_factory.hpp"
#include "texture.hpp"
#include "vector3.hpp"

namespace eng
{

sprite::sprite(std::shared_ptr<entity> e)
    : entity_(e)
{ }

sprite::sprite(
    const float x,
    const float y,
    const float width,
    const float height,
    const vector3 &colour)
    : sprite(x, y, width, height ,colour, texture::blank())
{ }

sprite::sprite(
    const float x,
    const float y,
    const float width,
    const float height,
    const vector3 &colour,
    texture &&tex)
    : entity_(shape_factory::sprite(x, y, width, height, colour, std::move(tex)))
{ }

std::shared_ptr<entity> sprite::renderable() const
{
    return entity_;
}

}

