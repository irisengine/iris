#include "mesh.hpp"

#include <cstdint>
#include <vector>

#include "matrix.hpp"
#include "vector3.hpp"

namespace eng
{

mesh::mesh(
    const std::vector<float> &vertices,
    const std::uint32_t colour,
    const vector3 &position,
    const float scale)
    : vertices_(vertices),
      colour_(colour),
      model_(matrix::make_translate(position) * matrix::make_scale(scale, scale))
{ }

void mesh::translate(const vector3 &t) noexcept
{
    model_ = matrix::make_translate(t) * model_;
}

const std::vector<float> mesh::vertices() const noexcept
{
    return vertices_;
}

std::uint32_t mesh::colour() const noexcept
{
    return colour_;
}

matrix mesh::model() const noexcept
{
    return model_;
}

}
