////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>

#include "graphics/animation/animation.h"
#include "graphics/animation/animation_layer.h"

namespace iris
{

class Skeleton;

/**
 * The AnimationController class provides the mechanism for animating skeletons. The base layer applies to all bones and
 * all subsequent layers can provide a mask to only animate specific bones. For example a skeleton could be running and
 * a layer which masks the arm bones could be used to play an attack animation. In the case where a non-looping
 * animation is applied to a masked layer a small easing is applied at the end to blend the bone back to the base layer
 * animation.
 */
class AnimationController
{
  public:
    /**
     * Construct a new AnimationController.
     *
     * @param animations
     *   Collection of animations to control. Note that the name of the animations is used as the identifier for all
     * other operations.
     *
     * @param layers
     *   Collection of layers. Note that the first element is considered the "bae layer" and must apply to all bones
     * i.e. cannot have a mask.
     *
     * @param skeleton
     *   The skeleton to apply animations to.
     */
    AnimationController(
        const std::vector<Animation> &animations,
        const std::vector<AnimationLayer> &layers,
        Skeleton &skeleton);

    ~AnimationController();

    /**
     * Updates the animations on all layers, should be called once per frame.
     */
    void update();

    /**
     * Start playing the named animation on the supplied layer. Note that if the name animation is not a valid
     * transition from the current animation this call is a no-op.
     *
     * @param layer
     *   Layer to play animation on.
     *
     * @param animation
     *   Name of animation to play.
     */
    void play(std::size_t layer, std::string_view animation);

  private:
    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
