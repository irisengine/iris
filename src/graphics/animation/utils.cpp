////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/animation/utils.h"

#include <chrono>

namespace iris
{

float blend_amount(std::chrono::milliseconds duration, std::chrono::system_clock::time_point end_time)
{
    const auto now = std::chrono::system_clock::now();
    const auto remaining = end_time - now;
    const auto remaining_percentage =
        static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(remaining).count()) /
        static_cast<float>(duration.count());
    return 1.0f - remaining_percentage;
}

}
