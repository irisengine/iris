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
    texture &&textures,
    const vector3 &position,
    const vector3 &scale)
    : vertices_(vertices),
      indices_(indices),
      texture_(std::move(textures)),
      model_(matrix4::make_translate(position) * matrix4::make_scale(scale)),
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

void mesh::translate(const vector3 &t) noexcept
{
    model_ = matrix4::make_translate(t) * model_;
}

void mesh::rotate(const quaternion &q) noexcept
{
    model_ = matrix4{ q } * model_;
}

const std::vector<vertex_data>& mesh::vertices() const noexcept
{
    return vertices_;
}

const std::vector<std::uint32_t>& mesh::indices() const noexcept
{
    return indices_;
}

matrix4 mesh::model() const noexcept
{
    return model_;
}

void mesh::set_model(const matrix4 &model) noexcept
{
    model_ = model;
}

}
