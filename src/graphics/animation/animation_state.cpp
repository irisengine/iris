////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/animation/animation_state.h"

#include <chrono>
#include <cstddef>
#include <map>
#include <string>
#include <string_view>

#include "graphics/animation/animation.h"
#include "graphics/animation/cached_bone_query.h"
#include "graphics/animation/utils.h"
#include "graphics/skeleton.h"
#include "log/log.h"

namespace iris
{

AnimationState::AnimationState(Animation *animation, Skeleton *skeleton, CachedBoneQuery *query, std::size_t layer)
    : skeleton_(skeleton)
    , query_(query)
    , animation_(animation)
    , transitions_()
    , next_state_(nullptr)
    , transition_complete_()
    , layer_(layer)
{
}

void AnimationState::enter()
{
    // nothing to do but can be used for logging
}

void AnimationState::exit()
{
    // state has exited so reset next state
    next_state_ = nullptr;
}

AnimationState *AnimationState::update()
{
    AnimationState *transition_state = nullptr;

    if (next_state_ != nullptr)
    {
        // we are transitioning to a new state

        const auto now = std::chrono::system_clock::now();

        if (now < transition_complete_)
        {
            // still transitioning so blend in current transition
            const auto duration = std::get<1>(transitions_[next_state_->animation_->name()]);
            query_->blend(layer_, next_state_->animation_, blend_amount(duration, transition_complete_));
        }
        else
        {
            // transition complete so update query with new state
            transition_state = next_state_;
            query_->update(layer_, next_state_->animation_);
        }
    }
    else
    {
        // no transition so just update animation
        query_->update(layer_, animation_);
    }

    skeleton_->update(query_);

    return transition_state;
}

void AnimationState::transition(std::string_view next)
{
    // if transition is valid then apply it, else no-op
    auto find_state = transitions_.find(next);
    if (find_state != std::end(transitions_))
    {
        const auto &[state, duration] = find_state->second;
        next_state_ = state;
        transition_complete_ = std::chrono::system_clock::now() + duration;

        next_state_->animation_->reset();
    }
}

Skeleton *AnimationState::skeleton() const
{
    return skeleton_;
}

CachedBoneQuery *AnimationState::bone_query() const
{
    return query_;
}

Animation *AnimationState::animation() const
{
    return animation_;
}

AnimationState::TransitionMap &AnimationState::transitions()
{
    return transitions_;
}

AnimationState *AnimationState::next_state() const
{
    return next_state_;
}

std::chrono::system_clock::time_point AnimationState::transition_complete() const
{
    return transition_complete_;
}

std::size_t AnimationState::layer() const
{
    return layer_;
}

}
