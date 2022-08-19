////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>

namespace iris
{

/**
 * Calculate the blend amount for the current time in an animation.
 *
 * @param duration
 *   Length of animation.
 *
 * @param end_time
 *   Time point when animation ends.
 *
 * @returns
 *   Blend amount for current progress in animation [0.0, 1.0].
 */
float blend_amount(std::chrono::milliseconds duration, std::chrono::system_clock::time_point end_time);

}
