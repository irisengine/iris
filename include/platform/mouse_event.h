#pragma once

namespace iris
{

/**
 * Encapsulates a mouse event. Stores delta x and y movement from previous
 * event.
 */
struct MouseEvent
{
    /**
     * Construct a new mouse Event with delta x and y coordinates.
     *
     * @param delta_x
     *   The amount the cursor has moved along the x-axis since the last event.
     *
     * @param delta_y
     *   The amount the cursor has moved along the y-axis since the last event.
     */
    MouseEvent(const float delta_x, const float delta_y)
        : delta_x(delta_x),
          delta_y(delta_y)
    { }

    /** Amount cursor has moved along x-axis since last mouse Event */
    float delta_x;

    /** Amount cursor has moved along y-axis since last mouse Event */
    float delta_y;
};

}

