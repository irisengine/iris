#include "mesh.hpp"

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
    : vertices_(vertices),
      indices_(indices),
      texture_(std::move(textures)),
      impl_(vertices_, indices_)
{ }

void mesh::bind() const
{
    impl_.bind();
    texture_.bind();
}

void mesh::unbind() const
{
    impl_.unbind();
    texture_.unbind();
}

const std::vector<vertex_data>& mesh::vertices() const noexcept
{
    return vertices_;
}

const std::vector<std::uint32_t>& mesh::indices() const noexcept
{
    return indices_;
}

}

