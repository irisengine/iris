////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "core/transform.h"
#include "graphics/animation/bone_query.h"

namespace iris
{

class Animation;
class Skeleton;

/**
 * Implementation fo BoneQuery that caches the transform of each bone. This allows us to blend from whatever the current
 * state is towards a new state. This is required form implementing transitions and layers.
 */
class CachedBoneQuery : public BoneQuery
{
  public:
    /**
     * Construct a new CachedBoneQuery.
     *
     * @param skeleton
     *   Skeleton to get bone names from.
     * @param bone_masks
     *   Collection of masks for bones per layer.
     */
    CachedBoneQuery(const Skeleton *skeleton, const std::vector<std::set<std::string>> &bone_masks);

    /**
     * Get the transformation for a bone.
     *
     * @param bone_name
     *   Name of the bone to query.
     *
     * @returns
     *   Bone transform, if bone exists.
     */
    std::optional<Transform> transform(std::string_view bone_name) override;

    /**
     * Update the bone transformations for the supplied layer.
     *
     * @param layer
     *   Layer to update.
     *
     * @param animation
     *   Animation to apply to bones.
     */
    void update(std::size_t layer, Animation *animation);

    /**
     * Blend the current state of the bones towards another animation.
     *
     * @param layer
     *   Layer to update.
     *
     * @param animation
     *   Animation to blend towards.
     *
     * @param blend_amount
     * . Amount to blend [0.0, 1.0].
     */
    void blend(std::size_t layer, Animation *animation, float blend_amount);

  private:
    /**
     * Internal struct for a cached bone.
     */
    struct CachedBone
    {
        std::size_t layer;
        Transform transform;
        std::chrono::system_clock::time_point ease_end;
    };

    /** Collection mapping bone name to bone data. */
    std::map<std::string, CachedBone, std::less<>> transforms_;
};

}