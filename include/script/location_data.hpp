#pragma once

#include <cstddef>

namespace eng
{

/**
 * Struct that encapsulates a position within a script. This is useful
 * for storing and presenting error information.
 */
struct location_data
{
    /**
     * Construct location_data object.
     *
     * @param line
     *   The line within the script (starts at 1).
     *
     * @param offset
     *   Character offset into line (starts at 0).
     */
    location_data(const std::size_t line, const std::size_t offset)
        : line(line),
          offset(offset)
    { }

    /**
     * Check of two line_data objects are equal.
     *
     * @param other
     *   Object to compare with.
     *
     *  returns
     *    True if objects are equal, false otherwise.
     */
    bool operator==(const location_data &other) const
    {
        return (line == other.line) && (offset == other.offset);
    }

    /**
     * Check of two line_data objects are not equal.
     *
     * @param other
     *   Object to compare with.
     *
     *  returns
     *    True if objects are not equal, false otherwise.
     */
    bool operator!=(const location_data &other) const
    {
        return !(*this == other);
    }

    /** Line  within script. */
    std::size_t line;

    /** Offset into line. */
    std::size_t offset;
};

}

