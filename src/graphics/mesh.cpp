#include "mesh.hpp"

#include <any>
#include <cstdint>
#include <utility>
#include <vector>

#include "matrix4.hpp"
#include "quaternion.hpp"
#include "vector3.hpp"
#include "vertex_data.hpp"

namespace eng
{

mesh::mesh(
    const std::vector<vertex_data> &vertices,
    const std::vector<std::uint32_t> &indices,
    texture &&textures)
    : indices_(indices),
      texture_(std::move(textures)),
      impl_(std::make_unique<mesh_implementation>(vertices, indices_))
{ }

const std::vector<std::uint32_t>& mesh::indices() const noexcept
{
    return indices_;
}

const buffer& mesh::vertex_buffer() const noexcept
{
    return impl_->vertex_buffer();
}

const buffer& mesh::index_buffer() const noexcept
{
    return impl_->index_buffer();
}

const texture& mesh::tex() const noexcept
{
    return texture_;
}

std::any mesh::native_handle() const
{
    return impl_->native_handle();
}

}

