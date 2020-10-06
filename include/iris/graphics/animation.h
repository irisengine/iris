#pragma once

#include <chrono>
#include <map>
#include <string>
#include <tuple>

#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/transform.h"
#include "core/vector3.h"
#include "graphics/keyframe.h"

namespace iris
{

/**
 * An animation represents a collection of keyframes for bones as well as
 * providing an interface for smoothly interpolating through them. An animation
 * will loop indefinitely.
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
        const std::map<std::string, std::vector<KeyFrame>> &frames);

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
    Transform transform(const std::string &bone) const;

    /**
     * Check if a bone exists in the animation.
     *
     * @param bone
     *   Bone name.
     *
     * @returns
     *   True if bone exists, false otherwise.
     */
    bool bone_exists(const std::string &bone) const;

    /**
     * Advances the animation by the amount of time since the last call.
     */
    void advance();

    /**
     * Reset animation time back to 0.
     */
    void reset();

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
    std::map<std::string, std::vector<KeyFrame>> frames_;
};

}
