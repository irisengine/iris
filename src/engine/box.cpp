#include "box.hpp"

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
 * @returns
 *   Inertia tensor of box.
 */
eng::matrix3 create_inertia_tensor(const float mass)
{
    const auto component = 0.3f * mass * (1.0f + 1.0f);

    return eng::matrix3{ { {
        component, 0.0f, 0.0f,
        0.0f, component, 0.0f,
        0.0f, 0.0f, component,
    } } };
}

}

namespace eng
{

box::box(
    const vector3 &position,
    const float mass,
    const bool is_static)
    : rigid_body(position, mass, create_inertia_tensor(mass), rigid_body_shape::BOX, is_static),
      half_size_({ 1.0f, 1.0f, 1.0f })
{ }

vector3 box::half_size() const noexcept
{
    return half_size_;
}

}

