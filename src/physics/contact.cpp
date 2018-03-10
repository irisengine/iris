#include "contact.hpp"

#include <cmath>

#include "matrix3.hpp"
#include "rigid_body.hpp"
#include "vector3.hpp"

namespace
{

/**
 * Helper function to calculate the velocity of a colliding body in contact
 * coordinates.
 *
 * @param body
 *   The body to calculate local velocity for.
 *
 * @param relative_contact_position
 *   Vector from centre of body to point of collision.
 *
 * @param contact_basis
 *   Used for transforming points into contact coordinates.
 *
 * @param delta
 *   Time step to calculate velocity for.
 *
 *
 * @returns
 *   The velocity of the body in contact coordinates.
 */
eng::vector3 local_velocity(
    eng::rigid_body * const body,
    const eng::vector3 &relative_contact_position,
    const eng::matrix3 &contact_basis)
{
    // velocity of contact point
    const auto velocity =
        eng::vector3::cross(body->angular_velocity(), relative_contact_position) +
        body->velocity();

    // convert to contact coordinates
    return eng::matrix3::transpose(contact_basis) * velocity;
}

}

namespace eng
{

contact::contact(
    const vector3 &position,
    const vector3 &normal,
    const float penetration,
    rigid_body * const body1,
    rigid_body * const body2)
    : position(position),
      normal(normal),
      penetration(penetration),
      body1(body1),
      body2(body2),
      contact_basis(matrix3::make_orthonormal_basis(normal)),
      relative_contact_position1(),
      relative_contact_position2(),
      closing_velocity(),
      desired_delta_velocity(0.0f)
{
    // calculate vectors from the centre of each body to the point of
    // contact
    relative_contact_position1 = position - body1->position();
    relative_contact_position2 = (!body2->is_static())
       ? position - body2->position()
       : vector3{ };

    // closing velocity is the local velocity of the first body
    closing_velocity = local_velocity(
        body1,
        relative_contact_position1,
        contact_basis);

    // if the second body is not static then account for that velocity
    if(!body2->is_static())
    {
        closing_velocity -= local_velocity(
            body2,
            relative_contact_position2,
            contact_basis);
    }

    update_desired_delta_velocity();
}

void contact::update_desired_delta_velocity()
{
    const auto restitution = 0.4f;

    // calculate delta required to remove closing velocity, taking into
    // account restitution
    desired_delta_velocity = -closing_velocity.x * (1.0f + restitution);
}

}

