#include "rigid_body.hpp"

#include <cmath>

#include "matrix3.hpp"
#include "matrix4.hpp"
#include "rigid_body_shape.hpp"
#include "quaternion.hpp"
#include "vector3.hpp"

namespace eng
{

rigid_body::rigid_body(
    const vector3 &position,
    const float mass,
    const matrix3 &inertia_tensor,
    const rigid_body_shape shape,
    const bool is_static)
    : inverse_mass_(1.0f / mass),
      position_(position),
      velocity_(),
      orientation_(),
      angular_velocity_(),
      acceleration_(),
      constant_acceleration_(),
      inverse_inertia_tensor_(),
      inverse_inertia_tensor_world_(),
      force_accumulator_(),
      torque_accumulator_(),
      linear_damping_(0.9f),
      angular_damping_(0.9f),
      transform_(),
      is_static_(is_static),
      shape_(shape)
{
    // compute inverse inertia tensor as that is what is used in calculations
    inverse_inertia_tensor_ = matrix3::invert(inertia_tensor);

    // ensure internal state is correct
    update();
}

rigid_body::~rigid_body()
{ }

void rigid_body::integrate(const float delta)
{
    // only integrate if not static
    if(!is_static())
    {
        // calculate acceleration on body
        acceleration_ = constant_acceleration_;
        acceleration_ += force_accumulator_ * inverse_mass_;

        // calculate angular acceleration on body
        const auto angular_acceleration =
            inverse_inertia_tensor_world_ * torque_accumulator_;

        // integrate using semi-implicit Euler integrator

        // integrate velocity and apply dampening
        velocity_ += (acceleration_ * delta);
        velocity_ *= std::pow(linear_damping_, delta);

        // integrate angular velocity and apply dampening
        angular_velocity_ += (angular_acceleration * delta);
        angular_velocity_ *= std::pow(angular_damping_, delta);

        // integrate position and orientation
        position_ += (velocity_ * delta);
        orientation_ += (angular_velocity_ * delta);

        // update internal state as position and orientation have most
        // likely changed
        update();

        // clear accumulators
        force_accumulator_ = vector3{ };
        torque_accumulator_ = vector3{ };
    }
}

void rigid_body::update()
{
    // ensure orientation is always normalised
    orientation_.normalise();

    // calculate transform matrix from position and orientation
    transform_ = matrix4{ orientation_, position_ };

    // cache a copy of the transform matrix as a matrix3
    const matrix3 transform_mat3{ transform_ };

    // update the world space inverse inertia tensor
    inverse_inertia_tensor_world_ =
        transform_mat3 * inverse_inertia_tensor_ * matrix3::transpose(transform_mat3);
}

void rigid_body::add_force(const vector3 &force)
{
    force_accumulator_ += force;
}

void rigid_body::add_torque(const vector3 &torque)
{
    torque_accumulator_ += torque;
}

vector3 rigid_body::add_impulse(const vector3 &impulse)
{
    const auto delta_velocity = impulse * inverse_mass_;
    velocity_ += delta_velocity;

    return delta_velocity;
}

vector3 rigid_body::add_angular_impulse(const vector3 &impulse)
{
    const auto delta_angular_velocity = inverse_inertia_tensor_world_ * impulse;
    angular_velocity_ += delta_angular_velocity;

    return delta_angular_velocity;
}

void rigid_body::set_constant_acceleration(const vector3 &acceleration)
{
    constant_acceleration_ = acceleration;
}

float rigid_body::mass() const
{
    return 1.0f / inverse_mass_;
}

float rigid_body::inverse_mass() const
{
    return inverse_mass_;
}

vector3 rigid_body::position() const
{
    return position_;
}

void rigid_body::warp_position(const vector3 &position)
{
    position_ = position;
    update();
}

vector3 rigid_body::velocity() const
{
    return velocity_;
}

vector3 rigid_body::angular_velocity() const
{
    return angular_velocity_;
}

quaternion rigid_body::orientation() const
{
    return orientation_;
}

vector3 rigid_body::acceleration() const
{
    return acceleration_;
}

void rigid_body::warp_orientation(const quaternion &orientation)
{
    orientation_ = orientation;
    update();
}

matrix4 rigid_body::transform() const
{
    return transform_;
}

matrix3 rigid_body::inverse_inertia_tensor_world() const
{
    return inverse_inertia_tensor_world_;
}

bool rigid_body::is_static() const
{
    return is_static_;
}

rigid_body_shape rigid_body::shape() const noexcept
{
    return shape_;
}

}

