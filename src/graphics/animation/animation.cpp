////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/animation/animation.h"

#include <map>
#include <string>
#include <string_view>
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
    const std::map<std::string, std::vector<KeyFrame>, std::less<>> &frames)
    : time_(duration) // this has the nice property that looping animations will start from the beginning (modulo) but
                      // single play animations will be stopped
    , last_advance_(std::chrono::steady_clock::now())
    , duration_(duration)
    , name_(name)
    , frames_(frames)
    , playback_type_(PlaybackType::LOOPING)
{
}

std::string Animation::name() const
{
    return name_;
}

Transform Animation::transform(std::string_view bone) const
{
    expect(bone_exists(bone), "no animation for bone");

    const auto &keyframes = frames_.find(bone)->second;

    // find the first keyframe *after* current time
    auto second_keyframe = std::find_if(
        std::cbegin(keyframes) + 1u, std::cend(keyframes), [this](const KeyFrame &kf) { return kf.time >= time_; });

    // if we are past the end of the animation then use the last keyframe
    if (second_keyframe == std::cend(keyframes))
    {
        --second_keyframe;
    }

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

bool Animation::bone_exists(std::string_view bone) const
{
    return frames_.count(bone) != 0u;
}

void Animation::advance()
{
    const auto now = std::chrono::steady_clock::now();
    const auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_advance_);

    // update time
    time_ += delta;

    if (playback_type_ == PlaybackType::LOOPING)
    {
        // looping animations wrap back round
        time_ = time_ % duration_;
    }

    last_advance_ = now;
}

void Animation::reset()
{
    time_ = 0ms;
    last_advance_ = std::chrono::steady_clock::now();
}

PlaybackType Animation::playback_type() const
{
    return playback_type_;
}

void Animation::set_playback_type(PlaybackType playback_type)
{
    playback_type_ = playback_type;
}

bool Animation::running() const
{
    return playback_type_ == PlaybackType::LOOPING ? true : (time_ < duration_);
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
