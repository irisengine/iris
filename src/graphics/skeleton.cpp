////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/skeleton.h"

#include <cassert>
#include <cstddef>
#include <queue>
#include <string>
#include <string_view>
#include <vector>

#include "core/error_handling.h"
#include "core/matrix4.h"
#include "graphics/animation.h"
#include "graphics/bone.h"
#include "graphics/bone_query.h"
#include "graphics/weight.h"

namespace
{

/**
 * Helper function to update transformation matrices. This is done by walking
 * the bone hierarchy and applying the bone transformations (optionally also
 * applied animation transformations).
 *
 * @param transforms
 *   Collection to update.
 *
 * @param bones
 *   Bones to calculate.
 *
 * @param parents
 *   Index of parents of bones.
 *
 * @param query
 *   Optional object to query bone transforms (for example from an animation).
 */
void update_transforms(
    std::vector<iris::Matrix4> &transforms,
    const std::vector<iris::Bone> &bones,
    const std::vector<std::size_t> &parents,
    iris::BoneQuery *query)
{
    // get inverse transform of root node
    const auto inverse = iris::Matrix4::invert(bones.front().transform());

    // we need some scratch space to save bone transformations as we calculate
    // them, this allows us to look up a parents transform
    // we don't want to update the actual bones transform as this causes issues
    // when we change animation
    std::vector<iris::Matrix4> cache(transforms.size());
    transforms[0] = bones.front().transform();

    // walk remaining bones - these are in hierarchal order so we will always
    // update a parent before its children
    for (auto i = 1u; i < bones.size(); ++i)
    {
        auto &bone = bones[i];

        if (bone.is_manual())
        {
            // if a bone is manual then its transform is absolute, so no need
            // to apply parents transform
            cache[i] = bone.transform();
            transforms[i] = inverse * cache[i] * bone.offset();
        }
        else if (query != nullptr)
        {
            // check if our bone exists in the supplied animation
            if (const auto transform = query->transform(bone.name()); transform)
            {
                // apply parent transform with animation transform
                cache[i] = cache[parents[i]] * transform->matrix();
                transforms[i] = inverse * cache[i] * bone.offset();
            }
        }
        else
        {
            // apply parent transform
            cache[i] = cache[parents[i]] * bone.transform();
            transforms[i] = inverse * cache[i] * bone.offset();
        }
    }
}

}

namespace iris
{

Skeleton::Skeleton()
    : Skeleton({{"root", {}, std::vector<Weight>{{0u, 1.0f}}, {}, {}}})
{
}

Skeleton::Skeleton(std::vector<Bone> bones)
    : bones_()
    , parents_()
    , transforms_(100)
{
    // a root bone is one without a parent, only support one
    ensure(
        std::count_if(std::cbegin(bones), std::cend(bones), [](const Bone &bone) { return bone.parent().empty(); }) ==
            1,
        "only support one root bones");

    auto root =
        std::find_if(std::begin(bones), std::end(bones), [](const Bone &bone) { return bone.parent().empty(); });

    // we need to copy the supplied bones in a specific order
    // the aim is to flatten the hierarchy so that the root node is first,
    // then its children, then grand children etc
    // this ordering guarantees that a nodes always precedes its children, which
    // makes updating transforms much simpler (as we can just iterate through
    // the list as we know a parents transformation will be updated by the time
    // we reach the child)

    std::queue<std::vector<Bone>::iterator> queue;
    queue.emplace(root);
    parents_.emplace_back(std::numeric_limits<std::size_t>::max());

    // breadth-first walk the hierarchy
    do
    {
        const auto iter = queue.front();
        queue.pop();

        // move the bone to the back of our list
        std::move(iter, iter + 1u, std::back_inserter(bones_));

        const auto name = bones_.back().name();

        // have to search all bones for children every time, not the most
        // efficient but its a one off cost
        for (auto i = std::begin(bones); i != std::end(bones); ++i)
        {
            if (i->parent() == name)
            {
                queue.emplace(i);
                parents_.emplace_back(bones_.size() - 1u);
            }
        }

    } while (!queue.empty());
}

const std::vector<Bone> &Skeleton::bones() const
{
    return bones_;
}

const std::vector<Matrix4> &Skeleton::transforms() const
{
    return transforms_;
}

void Skeleton::update(BoneQuery *query)
{
    update_transforms(transforms_, bones_, parents_, query);
}

std::size_t Skeleton::bone_index(std::string_view name) const
{
    const auto bone =
        std::find_if(std::cbegin(bones_), std::cend(bones_), [&name](const Bone &bone) { return bone.name() == name; });

    expect(bone != std::cend(bones_), "unknown bone");

    return std::distance(std::cbegin(bones_), bone);
}

Bone &Skeleton::bone(std::size_t index)
{
    return bones_[index];
}

Matrix4 Skeleton::transform(std::size_t index) const
{
    return transforms_[index];
}

const Bone &Skeleton::bone(std::size_t index) const
{
    return bones_[index];
}

}
