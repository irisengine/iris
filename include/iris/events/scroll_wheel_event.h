////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace iris
{

/**
 * Encapsulates a scroll wheel event. Stores delta y movement from previous * event.
 */
struct ScrollWheelEvent
{
    /**
     * Construct a new scroll whell Event with delta y coordinates.
     *
     * @param delta_y
     *   The amount the scroll has moved along the y-axis since the last event.
     */
    ScrollWheelEvent(const float delta_y)
        : delta_y(delta_y)
    {
    }

    /** Amount scroll wheel has moved along y-axis since last Event. */
    float delta_y;
};

}
