#pragma once

#include <cstdint>

namespace eng
{

/**
 * Enumeration of file system event types.
 */
enum class file_event_type : std::int8_t
{
    MODIFIED,
    DELETED
};

}

