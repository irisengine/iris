#include "box.hpp"

#include <cmath>

#include "matrix3.hpp"
#include "rigid_body.hpp"
#include "rigid_body_shape.hpp"
#include "vector3.hpp"

namespace
{

/**
 * Helper function to calculate inertia tensor of box.
 *
 * @param mass
 *   Mass of box.
 *
 * @param half_size
 *   The half_size of the box.
 *
 * @returns
 *   Inertia tensor of box.
 */
eng::matrix3 create_inertia_tensor(
    const float mass,
    const eng::vector3 &half_size)
{
    return eng::matrix3{ { {
        0.3f * mass * (std::pow(half_size.y, 2.0f) + std::pow(half_size.z, 2.0f)), 0.0f, 0.0f,
        0.0f, 0.3f * mass * (std::pow(half_size.x, 2.0f) + std::pow(half_size.z, 2.0f)), 0.0f,
        0.0f, 0.0f, 0.3f * mass * (std::pow(half_size.x, 2.0f) + std::pow(half_size.y, 2.0f)),
    } } };
}

}

namespace eng
{

box::box(
    const vector3 &position,
    const float mass,
    const vector3 &half_size,
    const bool is_static)
    : rigid_body(position, mass, create_inertia_tensor(mass, half_size), rigid_body_shape::BOX, is_static),
      half_size_(half_size)
{ }

vector3 box::half_size() const noexcept
{
    return half_size_;
}

}

