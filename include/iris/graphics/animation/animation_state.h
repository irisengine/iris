////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <cstddef>
#include <map>
#include <string>
#include <string_view>

namespace iris
{

class Animation;
class Skeleton;
class CachedBoneQuery;

/**
 * Internal class for a state in an animation state machine. Each state represents an animation and a
 * possible transitions to other states.
 */
class AnimationState
{
  public:
    // helper alias
    using TransitionMap = std::map<std::string, std::tuple<AnimationState *, std::chrono::milliseconds>, std::less<>>;

    /**
     * Construct a new AnimationState.
     *
     * @param animation
     *   Animation for this state.
     *
     * @param skeleton
     *   Skeleton to apply animation to (via the query object).
     *
     * @param query
     *   Query object to apply animations to.
     *
     * @param layer
     *   Which layer this state is in.
     */
    AnimationState(Animation *animation, Skeleton *skeleton, CachedBoneQuery *query, std::size_t layer);

    /**
     * Called when state is entered.
     */
    void enter();

    /**
     * Called when state is exited.
     */
    void exit();

    /**
     * Update the animation in this state.
     *
     * @returns
     *   If this state is complete and should transition to a new state, otherwise nullptr.
     */
    AnimationState *update();

    /**
     * Set the intention to transition to a new state, applying any transition time. If the transition is not valid then
     * this call is a no-op.
     *
     * @param next
     *   The name of the state to transition to.
     */
    void transition(std::string_view next);

    /**
     * Get the skeleton.
     *
     * @returns
     *   Skeleton.
     */
    Skeleton *skeleton() const;

    /**
     * Get the skeleton.
     *
     * @returns
     *   Skeleton.
     */
    CachedBoneQuery *bone_query() const;

    /**
     * Get the skeleton.
     *
     * @returns
     *   Skeleton.
     */
    Animation *animation() const;

    /**
     * Get the transitions.
     *
     * @returns
     *   Transitions.
     */
    TransitionMap &transitions();

    /**
     * Get the next state.
     *
     * @returns
     *   Next state.
     */
    AnimationState *next_state() const;

    /**
     * Get when the transition will be completed.
     *
     * @returns
     *   Completion time point of transition.
     */
    std::chrono::system_clock::time_point transition_complete() const;

    /**
     * Get the layer.
     *
     * @returns
     *   Layer of state.
     */
    std::size_t layer() const;

  private:
    /** Skeleton to update (via query) */
    Skeleton *skeleton_;

    /** Query to update. */
    CachedBoneQuery *query_;

    /** Animation for state. */
    Animation *animation_;

    /** Collection of transitions. */
    TransitionMap transitions_;

    /** State to transition to. */
    AnimationState *next_state_;

    /** Time point when transition should be complete. */
    std::chrono::system_clock::time_point transition_complete_;

    /** Layer of state. */
    std::size_t layer_;
};

}
