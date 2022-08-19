////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "core/matrix4.h"
#include "core/transform.h"
#include "core/utils.h"
#include "graphics/bone.h"

namespace iris
{

class BoneQuery;

/**
 * A skeleton provides an interface for animating bones.
 *
 * Internally bones are ordered into a tree hierarchy, setting an animation will
 * then walk the tree, applying transformations to bones and its children.
 *
 * A skeleton can only have a single root note (i.e. one without a parent).
 *
 * The internal order of bones is important (and may be different to the input
 * order), therefore an interface is also provided for querying bone index.
 *
 * The following diagram shows a skeleton and the bone hierarchy.
 *
 *                    +---+
 *                    | O | ---------- head
 * .                  +---+
 *                    | . | ---------- neck
 *                    +---+
 *              +---+ +---+ +---+
 * left_arm --- |  /| | | | |\ .| --- right_arm
 *              | / | | | | |  \|
 *              +---+ | | | +---+
 *                    | | |
 *                    | | | --------- spine
 *                    +---+
 *                    | . | --------- hip
 *                    +---+
 *              +---+       +---+
 * left_leg --- |  /|       |\ .| --- right_leg
 *              | / |       |  \|
 *              +---+       +---+
 * spine (root)
 * |
 * +--- neck
 * |    |
 * |    '--- head
 * |
 * '--- right_arm
 * |
 * '--- left_arm
 * |
 * '--- hip
 *      |
 *      '--- right_leg
 *      |
 *      '--- left_leg
 *
 */
class Skeleton
{
  public:
    /**
     * Construct an empty skeleton.
     */
    Skeleton();

    /**
     * Construct a skeleton.
     *
     * @param bones
     *   Collection of bones, these will be reordered.
     */
    Skeleton(std::vector<Bone> bones);

    /**
     * Get reference to collection of bones.
     *
     * @returns
     *   Reference to bone collection.
     */
    const std::vector<Bone> &bones() const;

    /**
     * Get reference to collection of transformation matrices for all bones (for
     * current animation). This is suitable for transferring to GPU.
     *
     * @returns
     *   Reference to bone transformations.
     */
    const std::vector<Matrix4> &transforms() const;

    /**
     * Update the bone transforms
     *
     * @param query
     *   Query object to get bone transforms (e.g. from an animation), can be nullptr.
     */
    void update(BoneQuery *query);

    /**
     * Check if a bone exists.
     *
     * @param name
     *   Name of bone to check.
     *
     * @returns
     *   True if bone exists, otherwise false.
     */
    bool has_bone(std::string_view name) const;

    /**
     * Get the index of the given bone name.
     *
     * @param name
     *   Name of bone.
     *
     * @returns
     *   Index of bone.
     */
    std::size_t bone_index(std::string_view name) const;

    /**
     * Get reference to bone at index.
     *
     * @param index
     *   Index of bone to get.
     *
     * @returns
     *   Reference to bone.
     */
    Bone &bone(std::size_t index);

    /**
     * Get transform for bone at index.
     *
     * @param index
     *   Index of bone to get.
     *
     * @returns
     *   Transform for bone.
     */
    Matrix4 transform(std::size_t index) const;

    /**
     * Get const reference to bone at index.
     *
     * @param index
     *   Index of bone to get.
     *
     * @returns
     *   Const reference to bone.
     */
    const Bone &bone(std::size_t index) const;

  private:
    /** Collection of bones, in hierarchical order. */
    std::vector<Bone> bones_;

    /** Index of parents for bones. */
    std::vector<std::size_t> parents_;

    /** Collection of transform matrices for bones. */
    std::vector<Matrix4> transforms_;
};

}
