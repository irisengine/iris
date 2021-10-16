////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <cstdint>

namespace iris
{

/**
 * Enumeration of touch event types.
 */
enum class TouchType : std::uint32_t
{
    BEGIN,
    MOVE,
    END
};

/**
 * Encapsulates a touch event. Stores screen position of event, type and a
 * unique id for the event.
 *
 * The id will be the same for all related touch events, i.e. a BEGIN, MOVE and
 * END event for the same touch will all have the same id. Id's are unique
 * amongst all active touches but maybe reused for future events.
 */
struct TouchEvent
{
    /**
     * Construct a new touch event.
     *
     * @param id
     *   Id for this event.
     *
     * @param type
     *   Type of event.
     *
     * @param x
     *   x coordinate in screen space of event.
     *
     * @param y
     *   y coordinate in screen space of event.
     */
    TouchEvent(std::uintptr_t id, TouchType type, float x, float y)
        : id(id)
        , type(type)
        , x(x)
        , y(y)
    {
    }

    /** Id of event. */
    std::uintptr_t id;

    /** Type of event. */
    TouchType type;

    /** x screen coordinate. */
    float x;

    /** y screen coordinate. */
    float y;
};

}
