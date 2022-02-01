////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "physics/basic_character_controller.h"

#include <chrono>
#include <vector>

#include "core/error_handling.h"
#include "core/vector3.h"
#include "log/log.h"
#include "physics/collision_shape.h"
#include "physics/contact_point.h"
#include "physics/physics_system.h"
#include "physics/rigid_body_type.h"

namespace iris
{

BasicCharacterController::BasicCharacterController(
    PhysicsSystem *physics_system,
    float speed,
    float width,
    float height,
    float float_height)
    : movement_direction_()
    , speed_(speed)
    , float_height_(float_height)
    , body_(nullptr)
{
    // use capsule shape for character
    body_ = physics_system->create_rigid_body(
        Vector3{0.0f, 0.0f, 10.0f},
        physics_system->create_capsule_collision_shape(width, height),
        RigidBodyType::CHARACTER_CONTROLLER);
}

void BasicCharacterController::set_movement_direction(const Vector3 &direction)
{
    movement_direction_ = direction;
}

Vector3 BasicCharacterController::position() const
{
    return body_->position();
}

Quaternion BasicCharacterController::orientation() const
{
    return body_->orientation();
}

void BasicCharacterController::set_speed(float speed)
{
    speed_ = speed;
}

void BasicCharacterController::reposition(const Vector3 &position, const Quaternion &orientation)
{
    body_->reposition(position, orientation);
}

RigidBody *BasicCharacterController::rigid_body() const
{
    return body_;
}

void BasicCharacterController::update(PhysicsSystem *ps, std::chrono::milliseconds delta)
{
    // simple three phase approach to updating a character controller

    const auto delta_seconds_fraction = static_cast<float>(delta.count()) / 1000.0f;

    // phase 1:
    // calculate the target position after the update based on the velocity and elapsed time (delta)

    const auto start_position = position();
    auto target_position = start_position + (movement_direction_ * speed_ * delta_seconds_fraction);

    // always update rigid body location position for future tests
    reposition(target_position, orientation());

    // phase 2:
    // ensure the target position is always a fixed distance above whatever object is below it

    // cast a ray down
    const auto rays_cast_results = ps->ray_cast(target_position, {0.0f, -1.0f, 0.0f}, {body_});
    if (!rays_cast_results.empty())
    {
        const auto hit = rays_cast_results.front();

        // if we are too far above our fixed distance then adjust the y component of the target position
        if ((hit.body->type() != RigidBodyType::GHOST) &&
            (Vector3::distance(hit.position, target_position) >= float_height_))
        {
            target_position.y = hit.position.y + float_height_;
        }
    }

    // always update rigid body location position for future tests
    reposition(target_position, orientation());

    static constexpr auto penetration_tolerace = 0.05f;
    static constexpr auto max_iterations = 5u;

    auto collisions_resolved = true;
    auto iteration = 0u;

    // phase 3:
    // check if target position is colliding with anything and attempt to resolve, if we cannot resolve then cancel the
    // movement this update

    do
    {
        collisions_resolved = true;

        // get all contacts with the body (which is at target position)
        const auto contacts = ps->contacts(body_);

        // find the first contact which is not a ghost and has a penetration above our tolerance
        // as contacts() provides us contact points sorted by penetration and we only want negative penetrations this
        // will also find us the largest penetration
        const auto valid_contact = std::find_if(
            std::cbegin(contacts),
            std::cend(contacts),
            [](const ContactPoint &cp)
            { return (cp.contact->type() != RigidBodyType::GHOST) && (cp.penetration > penetration_tolerace); });

        // if we found a valid contact then resolve
        if (valid_contact != std::cend(contacts))
        {
            // move target position back along penetration direction
            target_position += valid_contact->normal * valid_contact->penetration;

            // always update rigid body location position for future tests
            reposition(target_position, orientation());

            collisions_resolved = false;
        }
    } while (!collisions_resolved && (iteration++ < max_iterations));

    // if we could not resolve collisions then revert the rigid body position and do nothing this update
    if (!collisions_resolved)
    {
        reposition(start_position, orientation());
    }
}

}
