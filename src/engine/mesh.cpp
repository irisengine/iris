#include "mesh.hpp"

#include <cstdint>
#include <vector>

namespace eng
{

mesh::mesh(const std::vector<float> &vertices, const std::uint32_t colour)
    : vertices_(vertices),
      colour_(colour)
{ }

const std::vector<float> mesh::vertices() const noexcept
{
    return vertices_;
}

std::uint32_t mesh::colour() const noexcept
{
    return colour_;
}

}
