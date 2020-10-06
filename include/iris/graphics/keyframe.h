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
