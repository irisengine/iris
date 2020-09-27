#include "graphics/mesh.h"

#include "graphics/texture.h"

namespace iris
{

Mesh::Mesh(BufferDescriptor buffer_descriptor)
    : Mesh(std::move(buffer_descriptor), Texture::blank())
{
}

Mesh::Mesh(BufferDescriptor buffer_descriptor, Texture texture)
    : buffer_descriptor_(std::move(buffer_descriptor))
    , texture_(std::move(texture))
{
}

const BufferDescriptor &Mesh::buffer_descriptor() const
{
    return buffer_descriptor_;
}

const Texture &Mesh::texture() const
{
    return texture_;
}
}
