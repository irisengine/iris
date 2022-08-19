////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/animation/cached_bone_query.h"

#include <chrono>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "core/transform.h"
#include "graphics/animation/animation.h"
#include "graphics/animation/bone_query.h"
#include "graphics/animation/utils.h"
#include "graphics/skeleton.h"

using namespace std::literals::chrono_literals;

namespace iris
{

CachedBoneQuery::CachedBoneQuery(const Skeleton *skeleton, const std::vector<std::set<std::string>> &bone_masks)
{
    // create an empty transform for all bones on layer 0 (the base layer)
    for (const auto &bone : skeleton->bones())
    {
        transforms_[bone.name()] = {0u, {}, {}};
    }

    // set the layer for any bones in the supplied mask
    // note this starts at layer 1 and goes up, so a mask in a higher layer will overwrite one from a lower layer
    for (auto i = 0u; i < bone_masks.size(); ++i)
    {
        for (const auto &bone : bone_masks[i])
        {
            transforms_[bone].layer = i + 1u;
        }
    }
}

std::optional<Transform> CachedBoneQuery::transform(std::string_view bone_name)
{
    auto find = transforms_.find(bone_name);
    return find != std::end(transforms_) ? std::optional<Transform>{find->second.transform} : std::nullopt;
}

void CachedBoneQuery::update(std::size_t layer, Animation *animation)
{
    // the logic for this is a little complex as we need to achieve several things:
    // - update bone transform for base layer
    // - update bone transform for supplied layer (if in mask)
    // - if upper layer is a non-looping animation and it has finished then ease it back to the base layer animation
    //   to prevent snapping

    // we need to know if this update caused a non-looping animation to finish, the easiest way is to check if
    // running() is true before *and* false after advance()
    const auto running_pre_advance = animation->running();

    animation->advance();

    const auto running_post_advance = animation->running();
    const auto animation_finished_this_update = running_pre_advance && !running_post_advance;

    // update all transforms
    for (auto &[name, data] : transforms_)
    {
        auto &[bone_layer, transform, ease_end] = data;

        // layer 0 (base layer) always gets updated, if an upper layer masks this bone then it will be overwritten,
        // this isn't the most efficient as we are doing an extra update on masked layers, but it makes logic
        // slightly simpler
        if ((layer == 0u || layer == bone_layer) && animation->bone_exists(name))
        {
            if (animation->running())
            {
                // animation is running so overwrite the base layer transform with the one for this layer
                transform = animation->transform(name);
            }
            else
            {
                // animation has ended - we have the base animation layer already in the transform (this is why we
                // pay the cost and be slightly inefficient) - however we don't want to just snap to it as this will
                // look weird
                // instead we want to ease the current state to the base layer animation over a short time
                const auto now = std::chrono::system_clock::now();
                static constexpr auto ease_duration = 250ms;

                if (animation_finished_this_update)
                {
                    // we finished the animation this update call, so so set the time we want the easing to end
                    ease_end = now + ease_duration;

                    // we want to interpolate here, otherwise we will get one frame where we are in the wring place
                    transform.interpolate(animation->transform(name), 1.0f - blend_amount(ease_duration, ease_end));
                }
                else if (now < ease_end)
                {
                    // still easing, so interpolate
                    transform.interpolate(animation->transform(name), 1.0f - blend_amount(ease_duration, ease_end));
                }
            }
        }
    }
}

void CachedBoneQuery::blend(std::size_t layer, Animation *animation, float blend_amount)
{
    // update supplied animation
    animation->advance();

    for (auto &[name, data] : transforms_)
    {
        auto &[bone_layer, transform, ease_end] = data;

        if ((layer == 0u || layer == bone_layer) && animation->bone_exists(name))
        {
            // layer 0 (base layer) always gets updated, if an upper layer masks this bone then it will be overwritten,
            // this isn't the most efficient as we are doing an extra update on masked layers
            transform.interpolate(animation->transform(name), blend_amount);
        }
    }
}

}
