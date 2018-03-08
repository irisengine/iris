#include "collision_detector.hpp"

#include <cmath>
#include <vector>
#include <limits>

#include "box.hpp"
#include "contact.hpp"
#include "plane.hpp"
#include "rigid_body_shape.hpp"
#include "rigid_body.hpp"

namespace
{

/**
 * Project a box onto an axis. This is used when applying the separating axis
 * theorem to box-box collision.
 *
 *                  axis
 *                   |
 *      *====*       | .
 *     * *    *      |/|\
 *    *   *    *     | |
 *   *     *====*    | | distance
 *    *   *    *     | |
 *     * *    *      |\|/
 *      *====*       | '
 *                   |
 *
 * @param box
 *   The box to project.
 *
 * @param axis
 *   The axis to project on.
 *
 * @returns
 *   Length of projection.
 */
float length_of_projection(
    eng::box &box,
    const eng::vector3 &axis)
{
    return
        box.half_size().x * std::abs(axis.dot(box.transform().column(0u))) +
        box.half_size().y * std::abs(axis.dot(box.transform().column(1u))) +
        box.half_size().z * std::abs(axis.dot(box.transform().column(2u)));
}

/**
 * Test to see if two boxes overlap when projects onto an axis. This helper
 * function also performs some additional book keeping.
 *
 * @param box1
 *   First box to test.
 *
 * @param box2
 *   Second box to test.
 *
 * @param axis
 *   Copy of axis to project onto.
 *
 * @param to_centre
 *   Vector from the centre of each box.
 *
 * @param index
 *   A number identifying which axis test this is.
 *
 * @param smallest_penetration
 *   The smallest penetration seen on the box so far. Will be updated if this
 *   test produces a smaller penetration.
 *
 * @param smallest_case
 *   The smallest index seen so far. Will be updated if this test produces a
 *   smaller penetration.
 *
 * @returns
 *   True if boxes overlap on projects axis, false otherwise.
 */
bool try_axis(
    eng::box &box1,
    eng::box &box2,
    eng::vector3 axis,
    const eng::vector3 &to_centre,
    std::uint32_t index,
    float &smallest_penetration,
    std::uint32_t &smallest_case)
{
    auto overlapped = false;

    if(std::pow(axis.magnitude(), 2.0f) < 0.0001f)
    {
        // we may have calculate axis by using the cross product of two vectors
        // so we ignore the case when these were parallel
        overlapped = true;
    }
    else
    {
        axis.normalise();

        // calculate overlap of box projections along axis
        const auto project1 = length_of_projection(box1, axis);
        const auto project2 = length_of_projection(box2, axis);
        const auto distance = std::abs(to_centre.dot(axis));
        const auto penetration = project1 + project2 - distance;

        if(penetration < 0.0f)
        {
            // no overlap
            overlapped = false;
        }
        else
        {
            // perform book keeping if we have found a smaller penetration
            if(penetration < smallest_penetration)
            {
                smallest_penetration = penetration;
                smallest_case = index;
            }

            overlapped = true;
        }
    }

    return overlapped;
}

/**
 * Helper function to calculate a contact object when a vertex of one box
 * intersects a face of another.
 *
 * @param box1
 *   Box with face intersection.
 *
 * @param box2
 *   Box with vertex intersection.
 *
 * @param to_centre
 *   Vector from the centre of each box.
 *
 * @param best
 *   The index of the projection which produced this collision.
 *
 * @param penetration.
 *   The distance the boxes are penetrated.
 *
 * @returns
 *   A contact object encapsulating the information at the penetration point.
 */
eng::contact vertex_face_contact(
    eng::box &box1,
    eng::box &box2,
    const eng::vector3 &to_centre,
    std::uint32_t best,
    const float penetration)
{
    // calculate the normal of contact
    auto normal = box1.transform().column(best);
    if(normal.dot(to_centre) > 0.0f)
    {
        normal = -normal;
    }

    // calculate position of colliding vertex
    auto vertex = box2.half_size();

    if(box2.transform().column(0u).dot(normal) < 0.0f)
    {
        vertex.x *= -1.0f;
    }

    if(box2.transform().column(1u).dot(normal) < 0.0f)
    {
        vertex.y *= -1.0f;
    }

    if(box2.transform().column(2u).dot(normal) < 0.0f)
    {
        vertex.z *= -1.0f;
    }

    // get vertex in world coordinates
    vertex = box2.transform() * vertex;

    return {
        vertex,
        normal,
        penetration,
        std::addressof(box1),
        std::addressof(box2)
    };
}

/**
 * Helper method to calculate the contact point when two boxes intersect edge
 * to edge.
 *
 * @param point1
 *   Centre point of colliding edge on box1.
 *
 * @param axis1
 *   The projected axis for intersection of box1.
 *
 * @param one_size
 *   The half length of the colliding edge in box1.
 *
 * @param point1
 *   Centre point of colliding edge on box1.
 *
 * @param axis1
 *   The projected axis for intersection of box1.
 *
 * @param two_size
 *   The half length of the colliding edge in box1.
 *
 * @param use_one
 *   Whether to use the mid point from box1 or box2.
 *
 * @returns
 *   Contact point between edges.
 */
eng::vector3 contact_point(
    const eng::vector3 &point1,
    const eng::vector3 &axis1,
    const float one_size,
    const eng::vector3 &point2,
    const eng::vector3 &axis2,
    const float two_size,
    bool use_one)
{
    eng::vector3 contact_point{ };

    const auto square_mag1 = std::pow(axis1.magnitude(), 2.0f);
    const auto square_mag2 = std::pow(axis2.magnitude(), 2.0f);
    const auto dp = axis2.dot(axis1);

    const auto to_st = point1 - point2;
    const auto dp_sta1 = axis1.dot(to_st);
    const auto dp_sta2 = axis2.dot(to_st);

    const auto denominator = square_mag1 * square_mag2 - std::pow(dp, 2.0f);

    if(std::abs(denominator) < 0.0001f)
    {
        contact_point = use_one
            ? point1
            : point2;
    }
    else
    {
        const auto mua = (dp * dp_sta2 - square_mag2 * dp_sta1) / denominator;
        const auto mub = (square_mag1 * dp_sta2 - dp * dp_sta1) / denominator;

        if((mua > one_size) ||
           (mua < -one_size) ||
           (mub > two_size) ||
           (mub < -two_size))
        {
            contact_point = use_one
                ? point1
                : point2;
        }
        else
        {
            const auto c1 = point1 + axis1 * mua;
            const auto c2 = point2 + axis2 * mub;

            contact_point = c1 * 0.5f + c2 * 0.5f;
        }
    }

    return contact_point;
}

/**
 * Helper function to calculate a contact object when two boxes collide edge to
 * edge.
 *
 * @param box1
 *   First colliding box.
 *
 * @param box2
 *   Second colliding box.
 *
 * @param penetration
 *   The distance the boxes are penetrated.
 *
 * @param to_centre
 *   Vector from the centre of each box.
 *
 * @param best
 *   The index of the projection which produced this collision.
 *
 * @param best_single_axis
 *   The index of the projection along the standard box axis that produced
 *   the smallest collision..
 *
 * @returns
 *   A contact object encapsulating the information at the penetration point.
 */
eng::contact edge_edge_contact(
    eng::box &box1,
    eng::box &box2,
    const float penetration,
    const eng::vector3 &to_centre,
    const std::uint32_t best,
    const std::uint32_t best_single_axis)
{
        // get the projected axis
        const auto axis_index1 = best / 3u;
        const auto axis_index2 = best % 3u;

        const auto axis1 = box1.transform().column(axis_index1);
        const auto axis2 = box2.transform().column(axis_index2);
        auto axis = eng::vector3::cross(axis1, axis2);
        axis.normalise();

        // ensure axis points from box1 to box2
        if(axis.dot(to_centre) > 0.0f)
        {
            axis = -axis;
        }

        // there are four parallel edges to the axis, find which one is
        // closest to the axis and therefore penetrating
        auto point_on_edge1 = box1.half_size();

        if(axis_index1 == 0u)
        {
            point_on_edge1.x = 0.0f;
        }
        else if(box1.transform().column(0u).dot(axis) > 0.0f)
        {
            point_on_edge1.x *= -1.0f;
        }

        if(axis_index1 == 1u)
        {
            point_on_edge1.y = 0.0f;
        }
        else if(box1.transform().column(1u).dot(axis) > 0.0f)
        {
            point_on_edge1.y *= -1.0f;
        }

        if(axis_index1 == 2u)
        {
            point_on_edge1.z = 0.0f;
        }
        else if(box1.transform().column(2u).dot(axis) > 0.0f)
        {
            point_on_edge1.z *= -1.0f;
        }

        // there are four parallel edges to the axis, find which one is
        // closest to the axis and therefore penetrating
        auto point_on_edge2 = box2.half_size();

        if(axis_index2 == 0u)
        {
            point_on_edge2.x = 0.0f;
        }
        else if(box2.transform().column(0u).dot(axis) > 0.0f)
        {
            point_on_edge2.x *= -1.0f;
        }

        if(axis_index2 == 1u)
        {
            point_on_edge2.y = 0.0f;
        }
        else if(box2.transform().column(1u).dot(axis) > 0.0f)
        {
            point_on_edge2.y *= -1.0f;
        }

        if(axis_index2 == 2u)
        {
            point_on_edge2.z = 0.0f;
        }
        else if(box2.transform().column(2u).dot(axis) > 0.0f)
        {
            point_on_edge2.z *= -1.0f;
        }

        // transform points to world coordinates
        point_on_edge1 = box1.transform() * point_on_edge1;
        point_on_edge2 = box2.transform() * point_on_edge2;

        // get the half length of the box in the direction of the axis
        auto size1 = 0.0f;
        if(axis_index1 == 0u)
        {
            size1 = box1.half_size().x;
        }
        else if(axis_index1 == 1u)
        {
            size1 = box1.half_size().y;
        }
        else if(axis_index1 == 2u)
        {
            size1 = box1.half_size().z;
        }

        // get the half length of the box in the direction of the axis
        auto size2 = 0.0f;
        if(axis_index2 == 0u)
        {
            size2 = box2.half_size().x;
        }
        else if(axis_index2 == 1u)
        {
            size2 = box2.half_size().y;
        }
        else if(axis_index2 == 2u)
        {
            size2 = box2.half_size().z;
        }

        const auto vertex = contact_point(
            point_on_edge1,
            axis1,
            size1,
            point_on_edge2,
            axis2,
            size2,
            best_single_axis > 2u);

        return {
            vertex,
            axis,
            penetration,
            std::addressof(box1),
            std::addressof(box2)
        };
}

std::vector<eng::contact> resolve_box_box(
    eng::box &box1,
    eng::box &box2)
{
    std::vector<eng::contact> contacts{ };

    const auto to_centre = box2.position() - box1.position();

    // set max default values so we can find mins
    auto penetration = std::numeric_limits<float>::max();
    auto best = std::numeric_limits<std::uint32_t>::max();
    auto best_single_axis = best;

    // cache the vectors for each column in both matrix transform matrices
    const eng::vector3 columns[] = {
        box1.transform().column(0u),
        box1.transform().column(1u),
        box1.transform().column(2u),
        box2.transform().column(0u),
        box2.transform().column(1u),
        box2.transform().column(2u)
    };

    do
    {
        // test for overlap between boxes when projected along each boxes
        // principle axis
        // if any axis returns false then by the separating axis theorem the
        // boxes don't collide
        // we keep track which test gives us the smallest penetration

        auto separating = false;

        for(auto i = 0u; i < 6u; ++i)
        {
            if(!try_axis(
                box1,
                box2,
                columns[i],
                to_centre,
                i,
                penetration,
                best))
            {
                separating = true;
                break;
            }
        }

        if(separating)
        {
            break;
        }

        best_single_axis = best;

        // now test for overlap between boxes when projected along the axis
        // perpendicular to each principle axis

        const eng::vector3 perpendicular[] = {
            eng::vector3::cross(columns[0u], columns[3u]),
            eng::vector3::cross(columns[0u], columns[4u]),
            eng::vector3::cross(columns[0u], columns[5u]),
            eng::vector3::cross(columns[1u], columns[3u]),
            eng::vector3::cross(columns[1u], columns[4u]),
            eng::vector3::cross(columns[1u], columns[5u]),
            eng::vector3::cross(columns[2u], columns[3u]),
            eng::vector3::cross(columns[2u], columns[4u]),
            eng::vector3::cross(columns[2u], columns[5u]),
        };

        for(auto i = 6u; i < 15u; ++i)
        {
            if(!try_axis(
                box1,
                box2,
                perpendicular[i],
                to_centre,
                i,
                penetration,
                best))
            {
                separating = true;
                break;
            }
        }

        if(separating)
        {
            break;
        }

        // ensure we found a best axis
        if(best == std::numeric_limits<std::uint32_t>::max())
        {
            break;
        }

        // we've got a collision, handle calculating the contact based upon
        // the projection axis which gave the smallest penetration
        if(best < 3)
        {
            // vertex of box two colliding with a face of box one
            contacts.emplace_back(vertex_face_contact(
                box1,
                box2,
                to_centre,
                best,
                penetration));
        }
        else if(best < 6)
        {
            // vertex of box one colliding with a face of box two
            contacts.emplace_back(vertex_face_contact(
                box2,
                box1,
                -to_centre,
                best - 3u,
                penetration));
        }
        else
        {
            // we've got an edge to edge collision
            contacts.emplace_back(edge_edge_contact(
                box1,
                box2,
                penetration,
                to_centre,
                best - 6u,
                best_single_axis));
        }

    } while(false);

    return contacts;
}

/**
 * Calculate a collection of contact objects for each point a box intersects a
 * plane.
 *
 * @param box
 *   Box to test for intersection.
 *
 * @param plane
 *   Plane to test for intersection.
 *
 * @returns
 *   Collection of contact objects.
 */
std::vector<eng::contact> resolve_box_plane(
    eng::box &box,
    eng::plane &plane)
{
    std::vector<eng::contact> contacts{ };

    // we test for box-plane intersection by treating each corner of the box
    // as a point and seeing if that intersects with the plane
    // this will therefor generate up to four contacts

    // position of each corner of a unit box in local coordinates
    static const eng::vector3 corners[8] = {
        { 1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f },
        { -1.0f, -1.0f, 1.0f },
        { 1.0f, 1.0f, -1.0f },
        { -1.0f, 1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f },
        { -1.0f, -1.0f, -1.0f },
    };

    for(auto corner : corners)
    {
        // scale the corner to the size of the box
        corner.x *= box.half_size().x;
        corner.y *= box.half_size().y;
        corner.z *= box.half_size().z;

        // transform corner to world coordinates
        corner = box.transform() * corner;

        // get distance from plane
        const auto distance = corner.dot(plane.normal());

        // if less than offset then we have a collision
        if(distance <= plane.offset())
        {
            const auto contact_point =
                (plane.normal() * (distance - plane.offset())) + corner;

            contacts.emplace_back(
                contact_point,
                plane.normal(),
                plane.offset() - distance,
                std::addressof(box),
                std::addressof(plane));
        }
    }

    return contacts;
}

}

namespace eng::collision
{

std::vector<contact> detect(rigid_body &body1, rigid_body &body2)
{
    std::vector<contact> contacts{ };

    // only check for collision if both bodies aren't static, weird things
    // happen if we don't check plus this allows of overlapping static bodies
    if(!(body1.is_static() && body2.is_static()))
    {
        // generate contacts using specific algorithm for each supported collision
        // pair

        if((body1.shape() == rigid_body_shape::BOX) &&
           (body2.shape() == rigid_body_shape::BOX))
        {
            // safe cast back to concrete objects as we have checked shape type
            auto &box1 = static_cast<box&>(body1);
            auto &box2 = static_cast<box&>(body2);

            // ensure that if one of the bodies is static then it is the second
            // argument, this ensures that the body2 field of the resulting contact
            // object points to the static body
            contacts = box2.is_static()
                ? resolve_box_box(box1, box2)
                : resolve_box_box(box2, box1);
        }
        else if((body1.shape() == rigid_body_shape::BOX) &&
                (body2.shape() == rigid_body_shape::PLANE))
        {
            // safe cast back to concrete objects as we have checked shape type
            auto &b = static_cast<box&>(body1);
            auto &p = static_cast<plane&>(body2);

            contacts = resolve_box_plane(b, p);
        }
        else if((body1.shape() == rigid_body_shape::PLANE) &&
                (body2.shape() == rigid_body_shape::BOX))
        {
            // safe cast back to concrete objects as we have checked shape type
            auto &p = static_cast<plane&>(body1);
            auto &b = static_cast<box&>(body2);

            contacts = resolve_box_plane(b, p);
        }
    }

    return contacts;
}

}

