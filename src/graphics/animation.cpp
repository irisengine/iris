////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/animation.h"

#include <map>
#include <string>
#include <tuple>

#include "core/error_handling.h"
#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/vector3.h"
#include "log/log.h"

using namespace std::chrono_literals;

namespace iris
{

Animation::Animation(
    std::chrono::milliseconds duration,
    const std::string &name,
    const std::map<std::string, std::vector<KeyFrame>> &frames)
    : time_(0ms)
    , last_advance_(std::chrono::steady_clock::now())
    , duration_(duration)
    , name_(name)
    , frames_(frames)
{
}

std::string Animation::name() const
{
    return name_;
}

Transform Animation::transform(const std::string &bone) const
{
    expect(bone_exists(bone), "no animation for bone");

    const auto &keyframes = frames_.at(bone);

    // find the first keyframe *after* current time
    const auto &second_keyframe = std::find_if(
        std::cbegin(keyframes) + 1u, std::cend(keyframes), [this](const KeyFrame &kf) { return kf.time >= time_; });

    expect(second_keyframe != std::cend(keyframes), "cannot find keyframe");

    const auto first_keyframe = second_keyframe - 1u;

    // calculate interpolation amount
    const auto delta1 = second_keyframe->time - first_keyframe->time;
    const auto delta2 = time_ - first_keyframe->time;
    const auto interpolation = static_cast<float>(delta2.count()) / static_cast<float>(delta1.count());

    // interpolate between frames
    auto transform = first_keyframe->transform;
    transform.interpolate(second_keyframe->transform, interpolation);

    return transform;
}

bool Animation::bone_exists(const std::string &bone) const
{
    return frames_.count(bone) != 0u;
}

void Animation::advance()
{
    const auto now = std::chrono::steady_clock::now();
    const auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_advance_);

    // update time, ensuring we wrap around so animation loops
    time_ = (time_ + delta) % duration_;

    last_advance_ = now;
}

void Animation::reset()
{
    time_ = 0ms;
    last_advance_ = std::chrono::steady_clock::now();
}

std::chrono::milliseconds Animation::duration() const
{
    return duration_;
}

void Animation::set_time(std::chrono::milliseconds time)
{
    time_ = time;
}

}
