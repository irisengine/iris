////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <string>
#include <string_view>

namespace iris
{

/**
 * Struct encapsulating all the data needed for an animation transition.
 */
struct AnimationTransition
{
    /**
     * Construct a new AnimationTransition.
     *
     * @param from
     *   Name of animation to transition from.
     *
     * @param to
     *   Name of animation to transition from.
     *
     * @param duration
     *   How long the transition should take.
     */
    AnimationTransition(std::string_view from, std::string_view to, std::chrono::milliseconds duration)
        : from(from)
        , to(to)
        , duration(duration)
    {
    }

    /** Name of animation to transition from. */
    std::string from;

    /** Name of animation to transition to. */
    std::string to;

    /** Length of transition. */
    std::chrono::milliseconds duration;
};

}
