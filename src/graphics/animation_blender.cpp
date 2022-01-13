////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/animation_blender.h"

#include <string_view>
#include <vector>

#include "core/transform.h"
#include "graphics/animation/animation.h"

namespace iris
{

AnimationBlender::AnimationBlender()
{
}

AnimationBlender::AnimationBlender(Animation *animation_a, Animation *animation_b, float blend_amount)
    : animation_a_(animation_a)
    , animation_b_(animation_b)
    , blend_amount_(blend_amount)
{
}

Transform AnimationBlender::transform(std::string_view bone) const
{
    auto transform_a = animation_a_->transform(bone);
    const auto transform_b = animation_b_->transform(bone);

    transform_a.interpolate(transform_b, blend_amount_);
    return transform_a;
}

bool AnimationBlender::bone_exists(std::string_view bone) const
{
    return animation_a_->bone_exists(bone) && animation_b_->bone_exists(bone);
}

void AnimationBlender::advance()
{
    animation_a_->advance();
    animation_b_->advance();
}

void AnimationBlender::set_blend_amount(float blend_amount)
{
    blend_amount_ = blend_amount;
}

}
