////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/transform.h"
#include "core/vector3.h"
#include "graphics/keyframe.h"

namespace iris
{

/**
 * Enumeration of possible animation playback types.
 */
enum class PlaybackType
{
    /** Animation should keep playing. */
    LOOPING,

    /** Animation should play once. */
    SINGLE
};

/**
 * An animation represents a collection of keyframes for bones as well as
 * providing an interface for smoothly interpolating through them. An animation
 * will loop indefinitely.
 * @brief
 */
class Animation
{
  public:
    /**
     * Construct a new Animation.
     *
     * @param duration
     *   Length of animation.
     *
     * @param name
     *   Name of animation, should be unique within a skeleton.
     *
     * @param frames
     *   Map of bone names to keyframes. Keyframes must be in time order.
     */
    Animation(
        std::chrono::milliseconds duration,
        const std::string &name,
        const std::map<std::string, std::vector<KeyFrame>, std::less<>> &frames);

    /**
     * Get animation name.
     *
     * @returns
     *   Animation name.
     */
    std::string name() const;

    /**
     * Get the Transformation for a bone at the current animation time. If the
     * time falls between two keyframes then this method will interpolate
     * between them.
     *
     * @param bone
     *   Bone name.
     *
     * @returns
     *   Transformation of supplied bone at current animation time.
     */
    Transform transform(std::string_view bone) const;

    /**
     * Check if a bone exists in the animation.
     *
     * @param bone
     *   Bone name.
     *
     * @returns
     *   True if bone exists, false otherwise.
     */
    bool bone_exists(std::string_view bone) const;

    /**
     * Advances the animation by the amount of time since the last call.
     */
    void advance();

    /**
     * Reset animation time back to 0.
     */
    void reset();

    /**
     * Get playback type.
     *
     * @returns
     *   Playback type.
     */
    PlaybackType playback_type() const;

    /**
     * Set new playback type.
     *
     * @param playback_type
     *   New playback type.
     */
    void set_playback_type(PlaybackType playback_type);

    /**
     * Get if the animation is currently running.
     *
     * @returns
     *   True if playback type is SINGLE and the animation is running, always true for LOOPING.
     */
    bool running() const;

    /**
     * Get animation duration.
     *
     * @returns
     *   Duration of animation.
     */
    std::chrono::milliseconds duration() const;

    /**
     * Set the current time of animation, must be in range [0, duration()].
     *
     * @param time
     *   New time.
     */
    void set_time(std::chrono::milliseconds time);

  private:
    /** Current time of animation. */
    std::chrono::milliseconds time_;

    /** When the animation was last advanced. */
    std::chrono::steady_clock::time_point last_advance_;

    /** Length of the animation. */
    std::chrono::milliseconds duration_;

    /** Name of animation. */
    std::string name_;

    /** Collection of bones and their keyframes. */
    std::map<std::string, std::vector<KeyFrame>, std::less<>> frames_;

    /** Type of playback. */
    PlaybackType playback_type_;
};

}
