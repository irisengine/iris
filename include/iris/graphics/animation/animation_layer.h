////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "graphics/animation/animation_transition.h"

namespace iris
{

/**
 * Struct encapsulating a layer of animation i.e. a masked set of transitions. An empty bone mask implies the
 * transitions should apply to all bones.
 */
struct AnimationLayer
{
    /**
     * Construct a new AnimationLayer with an empty bone mask.
     *
     * @param transitions
     *   Transitions for this layer.
     *
     * @param start_animation
     *   Name of starting animation.
     */
    AnimationLayer(const std::vector<AnimationTransition> &transitions, std::string_view start_animation)
        : AnimationLayer({}, transitions, start_animation)
    {
    }

    /**
     * Construct a new AnimationLayer with a bone mask.
     *
     * @param bone_mask
     *   Collection of bone names to apply transitions to.
     *
     * @param transitions
     *   Transitions for this layer.
     *
     * @param start_animation
     *   Name of starting animation.
     */
    AnimationLayer(
        const std::vector<std::string> &bone_mask,
        const std::vector<AnimationTransition> &transitions,
        std::string_view start_animation)
        : bone_mask(bone_mask)
        , transitions(transitions)
        , start_animation(start_animation)
    {
    }

    /** Name of bones to apply transitions to. */
    std::vector<std::string> bone_mask;

    /** Collection of transitions to apply to masked bones. */
    std::vector<AnimationTransition> transitions;

    /** Name of starting animation. */
    std::string start_animation;
};

}
