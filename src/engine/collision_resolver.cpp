#include "collision_resolver.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <vector>

#include "contact.hpp"
#include "matrix3.hpp"
#include "vector3.hpp"

namespace
{

/**
 * Helper function to calculate the inertia of an object in the direction of
 * its contact normal.
 *
 * @param body
 *   The body to calculate inertia for.
 *
 * @param relative_contact_position
 *   Vector from centre of body to point of contact.
 *
 * @param contact_normal
 *   Normal of contact direction.
 *
 *  @returns
 *    A tuple of angular and linear inertia of body in direction of contact
 *    normal. If body is static then 0.0 is returned for both.
 */
std::tuple<float, float> calculate_inertia(
    const eng::rigid_body * const body,
    const eng::vector3 &relative_contact_position,
    const eng::vector3 &contact_normal)
{
    auto angular_inertia = 0.0f;
    auto linear_inertia = 0.0f;

    if(!body->is_static())
    {
        const auto inverse_inertia_tensor = body->inverse_inertia_tensor_world();

        // use inverse inertia tensor to calculate angular inertia
        auto angular_inertia_world = eng::vector3::cross(
            relative_contact_position,
            contact_normal);
        angular_inertia_world = inverse_inertia_tensor * angular_inertia_world;
        angular_inertia_world.cross(relative_contact_position);

        angular_inertia = angular_inertia_world.dot(contact_normal);

        // linear inertia is just inverse mass
        linear_inertia = body->inverse_mass();
    }

    return std::make_tuple(angular_inertia, linear_inertia);
}

/**
 * Helper function to actually apply a position and orientation change to
 * a rigid body so that it is no longer penetrating.
 *
 * @param sign
 *   A multiplier that allows the caller to change the direction this function
 *   is applied in, should either be 1.0 or -1.0.
 *
 * @param body
 *   Body to move.
 *
 * @param penetration
 *   Amount body is penetrated.
 *
 * @param total_inertia
 *   Total inertia from both colliding bodies.
 *
 * @param angular_inerita
 *   This bodies angular inertia.
 *
 * @param linear_inertia
 *   This bodies linear inertia.
 *
 * @param relative_contact_position
 *   Vector from centre of body to point of contact.
 *
 * @param contact_normal
 *   Normal of contact direction.
 *
 * @returns
 *   A tuple containing the change in angular and linear velocity. If the body
 *   is static then zero vectors are returned.
 */
std::tuple<eng::vector3, eng::vector3> apply_change(
    const float sign,
    eng::rigid_body * const body,
    const float penetration,
    const float total_inertia,
    const float angular_inertia,
    const float linear_inertia,
    const eng::vector3 &relative_contact_position,
    const eng::vector3 &contact_normal)
{
    eng::vector3 angular_change{ };
    eng::vector3 linear_change{ };

    if(!body->is_static())
    {
        // calculate how much to move body
        auto angular_move = sign * penetration * (angular_inertia / total_inertia);
        auto linear_move = sign * penetration * (linear_inertia / total_inertia);

        // calculate a maximum magnitude for angular movement, this prevents
        // movement that would be too large
        auto projection = relative_contact_position;
        projection += contact_normal * -relative_contact_position.dot(contact_normal);
        static const auto angular_limit = 0.2f;
        const auto max_magnitude = angular_limit * projection.magnitude();

        // ensure angular movement is within maximum
        if(angular_move < -max_magnitude)
        {
            const auto total_move = angular_move + linear_move;
            angular_move = -max_magnitude;
            linear_move = total_move - angular_move;
        }
        else if(angular_move > max_magnitude)
        {
            const auto total_move = angular_move + linear_move;
            angular_move = max_magnitude;
            linear_move = total_move - angular_move;
        }

        if(angular_move != 0.0f)
        {
            // calculate direction to rotate
            const auto target_angular_direction =
                eng::vector3::cross(relative_contact_position, contact_normal);
            const auto inverse_inertia_tensor = body->inverse_inertia_tensor_world();

            // calculate how much to rotate
            angular_change =
                (inverse_inertia_tensor * target_angular_direction) *
                (angular_move / angular_inertia);
        }

        linear_change = contact_normal * linear_move;

        // apply changes to position and orientation

        const auto position = body->position() + linear_change;
        body->warp_position(position);

        const auto orientation = body->orientation() + angular_change;
        body->warp_orientation(orientation);
    }

    return std::make_tuple(angular_change, linear_change);
};

/**
 * Helper function to apply a position change to both bodies in a supplied
 * contact so that they are no longer penetrating. This will make changes to
 * both position and orientation.
 *
 * @param contact
 *   The contact to resolve.
 *
 * @returns
 *   Tuple of changes to both angular and linear velocity for both bodies.
 */
std::tuple<eng::vector3, eng::vector3, eng::vector3, eng::vector3> apply_position_change(
    eng::contact &contact)
{
    const auto &[angular_inertia1, linear_inertia1] =
        calculate_inertia(contact.body1, contact.relative_contact_position1, contact.normal);

    const auto &[angular_inertia2, linear_inertia2] =
        calculate_inertia(contact.body2, contact.relative_contact_position2, contact.normal);

    // calculate total inertia
    const auto total_inertia =
        angular_inertia1 + linear_inertia1 +
        angular_inertia2 + linear_inertia2;

    // apply change to body1
    const auto [angular_change1, linear_change1] = apply_change(
        1.0f,
        contact.body1,
        contact.penetration,
        total_inertia,
        angular_inertia1,
        linear_inertia1,
        contact.relative_contact_position1,
        contact.normal);

    // apply change to body2
    const auto [angular_change2, linear_change2] = apply_change(
        -1.0f,
        contact.body2,
        contact.penetration,
        total_inertia,
        angular_inertia2,
        linear_inertia2,
        contact.relative_contact_position2,
        contact.normal);

    return std::make_tuple(
        angular_change1,
        linear_change1,
        angular_change2,
        linear_change2);
}

/**
 * Helper function to resolve penetrations for all supplied contacts.
 *
 * @param contacts
 *   Collection of contacts to resolve.
 */
void resolve_penetrations(std::vector<eng::contact> &contacts)
{
    // we need to resolve the penetration for all contacts
    // just doing this in the order they were generated can give unrealistic
    // looking results, so we do it in order of most to least penetration
    // we cannot simply sort the list though as resolving one contact may
    // change the penetration of another, in this sense we cannot know which
    // contact to do next (without searching) or how many iteration we will
    // need
    // rather than looping until all resolutions are solved (which could take
    // some time) we place an upper limit on the amount of iterations, we assume
    // that any remaining penetrations after this will be too small to notice
    for(auto i = 0u; i < contacts.size() * 2u; ++i)
    {
        // find contact with largest penetration
        auto largest = std::max_element(
            std::begin(contacts),
            std::end(contacts),
            [](const auto &element1, const auto &element2)
            {
                return element1.penetration < element2.penetration;
            });

        // check that we still have a penetration left to resolve, we use a min
        // penetration value for numerical stability
        static const auto min_penetration = 0.01f;
        if((largest == std::cend(contacts)) || ((*largest).penetration < min_penetration))
        {
            break;
        }

        // apply the change in position so both penetrating bodies are
        // separated and keep track of the change in linear and angular velocity
        // required for this
        const auto [angular_change1, linear_change1, angular_change2, linear_change2] =
            apply_position_change(*largest);

        // we now have to update any other penetrations that would have
        // occurred because of this resolution
        // it is too expensive to re-run the entire collision detection
        // algorithm, so we make the following approximation: if any other
        // contact involved either of the bodies just moved then update their
        // position based on the stored change in linear and angular velocity
        for(auto j = 0u; j < contacts.size(); ++j)
        {
            const auto *body1 = contacts[j].body1;
            const auto *body2 = contacts[j].body2;

            // update penetrations if the contact involved the first body
            if(body1 == (*largest).body1)
            {
                const auto delta_position =
                    linear_change1 +
                    eng::vector3::cross(angular_change1, contacts[j].relative_contact_position1);

                contacts[j].penetration -= delta_position.dot(contacts[j].normal);
            }
            else if(body1 == (*largest).body2)
            {
                const auto delta_position =
                    linear_change2 +
                    eng::vector3::cross(angular_change2, contacts[j].relative_contact_position1);

                contacts[j].penetration -= delta_position.dot(contacts[j].normal);
            }

            // update penetrations if the contact involved the second body (and
            // its not static)
            if(!body2->is_static())
            {
                if(body2 == (*largest).body1)
                {
                    const auto delta_position =
                        linear_change1 +
                        eng::vector3::cross(angular_change1, contacts[j].relative_contact_position2);

                    contacts[j].penetration += delta_position.dot(contacts[j].normal);
                }
                else if(body2 == (*largest).body2)
                {
                    const auto delta_position =
                        linear_change2 +
                        eng::vector3::cross(angular_change2, contacts[j].relative_contact_position2);

                    contacts[j].penetration += delta_position.dot(contacts[j].normal);
                }
            }
        }
    }
}

/**
 * Helper function to calculate the impulse for a friction based contact.
 *
 * @param inverse_inertia_tensor1
 *   The inverse inertia tensor for the first colliding body.
 *
 * @param inverse_inertia_tensor2
 *   The inverse inertia tensor for the second colliding body.
 *
 * @param contact
 *   Contact to resolve.
 *
 * @returns
 *   Impulse for collision with friction.
 */
eng::vector3 calculate_friction_impulse(
    const eng::matrix3 &inverse_inertia_tensor1,
    const eng::matrix3 &inverse_inertia_tensor2,
    const eng::contact &contact)
{
    eng::vector3 impulse_contact{ };
    auto inverse_mass = contact.body1->inverse_mass();

    // create matrix for converting between linear and angular quantities
    auto impulse_to_torque =
        eng::matrix3::make_skew_symmetric(contact.relative_contact_position1);

    // create matrix for converting contact impulse to verlocity
    auto delta_velocity_world = impulse_to_torque;
    delta_velocity_world *= inverse_inertia_tensor1;
    delta_velocity_world *= impulse_to_torque;
    delta_velocity_world *= -1.0f;

    // add data from body2 if needed
    if(!contact.body2->is_static())
    {
        impulse_to_torque =
            eng::matrix3::make_skew_symmetric(contact.relative_contact_position2);

        auto delta_velocity_world2 = impulse_to_torque;
        delta_velocity_world2 *= inverse_inertia_tensor2;
        delta_velocity_world2 *= impulse_to_torque;
        delta_velocity_world2 *= -1.0f;

        delta_velocity_world += delta_velocity_world2;
        inverse_mass += contact.body2->inverse_mass();
    }

    // convert delta velocity world into contact coordinates
    auto delta_velocity = eng::matrix3::transpose(contact.contact_basis);
    delta_velocity *= delta_velocity_world;
    delta_velocity *= contact.contact_basis;

    // add linear velocity change
    delta_velocity[0u] += inverse_mass;
    delta_velocity[4u] += inverse_mass;
    delta_velocity[8u] += inverse_mass;

    const auto impulse_matrix = eng::matrix3::invert(delta_velocity);

    // find target velocity to kill
    const eng::vector3 velocity_kill{
        contact.desired_delta_velocity,
        -contact.closing_velocity.y,
        -contact.closing_velocity.z };

    impulse_contact = impulse_matrix * velocity_kill;

    const auto planar_impulse = std::sqrt(
        std::pow(impulse_contact.y, 2.0f) + std::pow(impulse_contact.z, 2.0f));

    static const auto friction = 1.0f;

    // if we exceed static friction then apply dynamic friction
    if(planar_impulse > (impulse_contact.x * friction))
    {
        impulse_contact.y /= planar_impulse;
        impulse_contact.z /= planar_impulse;

        impulse_contact.x =
            delta_velocity[0u] +
            delta_velocity[1u] * friction * impulse_contact.y +
            delta_velocity[2u] * friction * impulse_contact.z;
        impulse_contact.x = contact.desired_delta_velocity / impulse_contact.x;
        impulse_contact.y *= friction * impulse_contact.x;
        impulse_contact.z *= friction * impulse_contact.x;
    }

    return impulse_contact;
}

/**
 * Helper function to apply impulses to a body.
 *
 * @param body
 *   Body to apply impulses to.
 *
 * @param sign
 *   A multiplier that allows the caller to change the direction this function
 *   is applied in, should either be 1.0 or -1.0.
 *
 * @param impulse
 *   Impulse in each contact axis.
 *
 * @param relative_contact_position
 *   Vector from centre of body to point of contact.
 *
 * @returns
 *   A tuple containing the change in rotation and velocity, if body is static
 *   these are both zero vectors.
 */
std::tuple<eng::vector3, eng::vector3> apply_impulses(
        eng::rigid_body * body,
        const float sign,
        const eng::vector3 &impulse,
        const eng::vector3 &relative_contact_position)
{
    eng::vector3 rotation_change{ };
    eng::vector3 velocity_change{ };

    if(!body->is_static())
    {
        const auto impulse_torque = sign > 0.0f
            ? eng::vector3::cross(relative_contact_position, impulse)
            : eng::vector3::cross(impulse, relative_contact_position);
        rotation_change = body->add_angular_impulse(impulse_torque);
        velocity_change = body->add_impulse(impulse * sign);
    }

    return std::make_tuple(rotation_change, velocity_change);
};

/**
 * Helper function to calculate and apply the velocity change to bodies in a
 * supplied contact.
 *
 * @param contact
 *   Contact to resolve.
 *
 * @returns
 *   Tuple containing the change in rotation and velocity for both bodies in
 *   the contact.
 */
std::tuple<eng::vector3, eng::vector3, eng::vector3, eng::vector3> apply_velocity_change(
    eng::contact &contact)
{
    const auto inverse_inertia_tensor_world1 = contact.body1->inverse_inertia_tensor_world();
    const auto inverse_inertia_tensor_world2 = (!contact.body2->is_static())
       ? contact.body2->inverse_inertia_tensor_world()
       : eng::matrix3{ {{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }} };

    // calculate impulse based on collision with friction
    const auto impulse_contact = calculate_friction_impulse(
        inverse_inertia_tensor_world1,
        inverse_inertia_tensor_world2,
        contact);

    // transform impulse into world coordinates
    const auto impulse = contact.contact_basis * impulse_contact;

    // apply impulses

    const auto [rotation_change1, velocity_change1] = apply_impulses(
        contact.body1,
        1.0f,
        impulse,
        contact.relative_contact_position1);

    const auto [rotation_change2, velocity_change2] = apply_impulses(
        contact.body2,
        -1.0f,
        impulse,
        contact.relative_contact_position2);

    return std::make_tuple(
        rotation_change1,
        velocity_change1,
        rotation_change2,
        velocity_change2);
}

/**
 * Helper function to adjust the velocities of colliding bodies.
 *
 * @param contacts
 *   Collection of contacts to resolve.
 */
void adjust_velocities(std::vector<eng::contact> &contacts)
{
    // we need to adjust the velocities for all contacts, such that their
    // desired delta velocities are all zero
    // just doing this in the order they were generated can give unrealistic
    // looking results, so we do it in order of most to least desired velocity
    // we cannot simply sort the list though as adjusting one contact may
    // change the desired velocity of another, in this sense we cannot know which
    // contact to do next (without searching) or how many iteration we will
    // need
    // rather than looping until all desired velocities are zero (which could take
    // some time) we place an upper limit on the amount of iterations, we assume
    // that any remaining desired velocities after this will be too small to notice
    for(auto i = 0u; i < contacts.size() * 2; ++i)
    {
        // find contact with largest delta velocity
        auto largest = std::max_element(
            std::begin(contacts),
            std::end(contacts),
            [](const auto &element1, const auto &element2)
            {
                return element1.desired_delta_velocity < element2.desired_delta_velocity;
            });

        // check that we still have a desired delta left to adjust, we use a min
        // delta value for numerical stability
        static const auto min_delta = 0.01f;
        if((largest == std::end(contacts)) || ((*largest).desired_delta_velocity <= min_delta))
        {
            break;
        }

        // apply velocity change and keep track of change in rotation and
        // velocity for each body
        const auto [rotation_change1, velocity_change1, rotation_change2, velocity_change2] =
            apply_velocity_change(*largest);

        // helper lambda to update closing velocity and desired delta velocity
        // for a contact
        const auto update_velocity = [](
            const eng::vector3 &velocity_change,
            const eng::vector3 &rotation_change,
            const eng::vector3 &relative_contact_position,
            const eng::matrix3 &contact_basis_transpose,
            eng::contact &contact,
            const float sign)
        {
            const auto delta_velocity =
                velocity_change +
                eng::vector3::cross(rotation_change, relative_contact_position);

            contact.closing_velocity += contact_basis_transpose * delta_velocity * sign;
            contact.update_desired_delta_velocity();
        };

        // if we have changed a bodies velocity then this could change the
        // closing velocity of another contact
        // recompute closing and desired delta velocity for all contact which
        // involve the two current bodes
        for(auto j = 0u; j < contacts.size(); ++j)
        {
            const auto contact_basis_transpose = eng::matrix3::transpose(contacts[j].contact_basis);
            const auto *body1 = contacts[j].body1;
            const auto *body2 = contacts[j].body2;

            // update of body1 is part of contact
            if(body1 == (*largest).body1)
            {
                update_velocity(
                    velocity_change1,
                    rotation_change1,
                    contacts[j].relative_contact_position1,
                    contact_basis_transpose,
                    contacts[j],
                    1.0f);
            }
            else if(body1 == (*largest).body2)
            {
                update_velocity(
                    velocity_change2,
                    rotation_change2,
                    contacts[j].relative_contact_position1,
                    contact_basis_transpose,
                    contacts[j],
                    1.0f);
            }

            // update of body2 is part of contact (and it is not static)
            if(!body2->is_static())
            {
                if(body2 == (*largest).body1)
                {
                    update_velocity(
                        velocity_change1,
                        rotation_change1,
                        contacts[j].relative_contact_position2,
                        contact_basis_transpose,
                        contacts[j],
                        -1.0f);
                }
                else if(body2 == (*largest).body2)
                {
                    update_velocity(
                        velocity_change2,
                        rotation_change2,
                        contacts[j].relative_contact_position2,
                        contact_basis_transpose,
                        contacts[j],
                        -1.0f);
                }
            }
        }
    }
}

}

namespace eng::collision
{

void resolve(std::vector<contact> &contacts)
{
    if(!contacts.empty())
    {
        // first stage is to resolve all penetrations, so move all objects apart
        // the smallest amount so that they are no longer colliding
        resolve_penetrations(contacts);

        // second stage is to apply any velocity changes to objects that have
        // occurred because of collision
        adjust_velocities(contacts);
    }
}

}

