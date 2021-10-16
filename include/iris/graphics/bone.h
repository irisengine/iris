////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "core/matrix4.h"
#include "core/transform.h"
#include "graphics/weight.h"

namespace iris
{

/**
 * A bone is a component of skeletal animation. It represents a series of
 * vertices it influences as well as the amount it influences them (weight).
 * Alternatively a bone may have zero weights, in this case it represents an
 * intermediate transformation in the bone hierarchy.
 *
 * A bone stores two matrices:
 *  offset - transforms vertices from local space to bone space
 *  transform - transforms vertices into the current bone pose
 *
 * During execution the offset is immutable but the transform will change as the
 * skeleton the bone is part of is animated.
 *
 * If a bone is set to "manual" then it's transform is absolute i.e. a Skeleton
 * will not apply its parent transformation. This is useful if you want to set
 * a position in world space.
 */
class Bone
{
  public:
    /**
     * Construct a new Bone.
     *
     * @param name
     *   Unique name of the bone.
     *
     * @param parent
     *   Unique name of the parent bone.
     *
     * @param weights
     *   Collection of Weight objects that define the bone. May be empty.
     *
     * @param offset
     *   Matrix which transforms vertices from local space to bone space.
     *
     * @param transform
     *   Initial matrix which transforms bone for an animation.
     */
    Bone(
        const std::string &name,
        const std::string &parent,
        const std::vector<Weight> &weights,
        const Matrix4 &offset,
        const Matrix4 &transform);

    /**
     * Get name of bone.
     *
     * @returns
     *  Bone name.
     */
    std::string name() const;

    /** Get name of parent pone.
     *
     * @returns
     *   Parent bone name.
     */
    std::string parent() const;

    /**
     * Get reference to collection of weights.
     *
     * @returns
     *   Reference to weights.
     */
    const std::vector<Weight> &weights() const;

    /**
     * Get reference to offset matrix.
     *
     * @returns
     *   Reference to offset matrix.
     */
    const Matrix4 &offset() const;

    /**
     * Get reference to transformation matrix.
     *
     * @returns
     *   Reference to transform matrix.
     */
    const Matrix4 &transform() const;

    /**
     * Set the transformation matrix.
     *
     * @param transform
     *   New transform.
     */
    void set_transform(const Matrix4 &transform);

    /**
     * Check if the bone is manual.
     *
     * @returns
     *   True if bone is manual, false otherwise.
     */
    bool is_manual() const;

    /**
     * Set if bone is manual.
     *
     * @param is_manual
     *   New manual value
     */
    void set_manual(bool is_manual);

  private:
    /** Bone name. */
    std::string name_;

    /** Parent bone name. */
    std::string parent_;

    /** Collection of weights. */
    std::vector<Weight> weights_;

    /** Offset matrix. */
    Matrix4 offset_;

    /** Transformation matrix. */
    Matrix4 transform_;

    /** If bone is manual. */
    bool is_manual_;
};

}
