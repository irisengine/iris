#include "graphics/mesh.h"

#include "graphics/texture.h"
#include "graphics/texture_factory.h"

namespace iris
{

Mesh::Mesh(BufferDescriptor buffer_descriptor)
    : Mesh(std::move(buffer_descriptor), texture_factory::blank())
{
}

Mesh::Mesh(BufferDescriptor buffer_descriptor, Texture *texture)
    : buffer_descriptor_(std::move(buffer_descriptor))
    , texture_(texture)
{
}

const BufferDescriptor &Mesh::buffer_descriptor() const
{
    return buffer_descriptor_;
}

const Texture *Mesh::texture() const
{
    return texture_;
}
}
