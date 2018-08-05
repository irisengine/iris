#pragma once

#include <memory>

#include "entity.hpp"
#include "material.hpp"
#include "quaternion.hpp"
#include "vector3.hpp"

namespace eng::shape_factory
{

/**
 * Create a box with the supplied half size. Will use an in build basic
 * material.
 *
 * @param half_size
 *   Half size of the box.
 *
 * @param colour
 *   The colour of the entity.
 *
 * @param position
 *   Position of entity in world space.
 *
 * @param orientation
 *   Orientation of entity.
 */
std::shared_ptr<entity> box(
    const vector3 &half_size,
    const vector3 &colour,
    const vector3 &position,
    const quaternion &orientation);

/**
 * Create a box with the supplied half size.
 *
 * @param half_size
 *   Half size of the box.
 *
 * @param colour
 *   The colour of the entity.
 *
 * @param position
 *   Position of entity in world space.
 *
 * @param orientation
 *   Orientation of entity.
 *
 * @param mat
 *   Material to render with.
 */
std::shared_ptr<entity> box(
    const vector3 &half_size,
    const vector3 &colour,
    const vector3 &position,
    const quaternion &orientation,
    std::shared_ptr<material> mat);

/**
 * Create a plane. Will use an in build basic material.
 *
 * @param colour
 *   The colour of the entity.
 *
 * @param position
 *   Position of entity in world space.
 */
std::shared_ptr<entity> plane(
    const vector3 &colour,
    const vector3 &position);

/**
 * Create a plane.
 *
 * @param colour
 *   The colour of the entity.
 *
 * @param position
 *   Position of entity in world space.
 *
 * @param mat
 *   Material to render with.
 */
std::shared_ptr<entity> plane(
    const vector3 &colour,
    const vector3 &position,
    std::shared_ptr<material> mat);

/**
 * Create a sphere with the supplied radius. Will use an in built basic
 * material.
 *
 * @param radius
 *   Radius of sphere.
 *
 * @param colour
 *   The colour of the entity.
 *
 * @param position
 *   Position of entity in world space.
 *
 * @param orientation
 *   Orientation of entity.
 */
std::shared_ptr<entity> sphere(
    const float radius,
    const vector3 &colour,
    const vector3 &position);

/**
 * Create a sphere with the supplied radius. Will use an in built basic
 * material.
 *
 * @param radius
 *   Radius of sphere.
 *
 * @param colour
 *   The colour of the entity.
 *
 * @param position
 *   Position of entity in world space.
 *
 * @param orientation
 *   Orientation of entity.
 *
 * @param mat
 *   Material to render with.
 */
std::shared_ptr<entity> sphere(
    const float radius,
    const vector3 &colour,
    const vector3 &position,
    std::shared_ptr<material> mat);

/**
 * Create a sprite with the supplied dimensions. Will use an in build basic
 * material.
 *
* @param x
*   Screen x coordinate of centre of sprite.
*
* @param y
*   Screen y coordinate of centre of sprite.
*
* @param width
*   Width of sprite.
*
* @param height
*   Height of sprite.
*
* @param colour
*   Colour of sprite.
*
* @param tex
*   Texture of sprite.
*/
std::shared_ptr<entity> sprite(
    const float x,
    const float y,
    const float width,
    const float height,
    const vector3 &colour,
    texture &&tex);

/**
 * Create a sprite with the supplied dimensions.
 *
* @param x
*   Screen x coordinate of centre of sprite.
*
* @param y
*   Screen y coordinate of centre of sprite.
*
* @param width
*   Width of sprite.
*
* @param height
*   Height of sprite.
*
* @param colour
*   Colour of sprite.
*
* @param tex
*   Texture of sprite.
 *
 * @param mat
 *   Material to render with.
*/
std::shared_ptr<entity> sprite(
    const float x,
    const float y,
    const float width,
    const float height,
    const vector3 &colour,
    texture &&tex,
    std::shared_ptr<material> mat);

}

