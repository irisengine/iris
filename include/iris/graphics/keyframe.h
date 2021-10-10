////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>

#include "core/transform.h"

namespace iris
{

/**
 * A KeyFrame represents a transformation at a given time in an animation.
 */
struct KeyFrame
{
    /**
     * Construct a new KeyFrame.
     *
     * @param transform
     *   Transform at time.
     *
     * @param time
     *   Time of keyframe (since start of animation).
     */
    KeyFrame(const Transform &transform, std::chrono::milliseconds time)
        : transform(transform)
        , time(time)
    {
    }

    /** Transform of frame. */
    Transform transform;

    /** Time of frame. */
    std::chrono::milliseconds time;
};

}
