#pragma once

#include <cstdint>

namespace eng
{

/**
 * Enumeration of states a fiber can be in.
 */
enum class FiberState : std::uint8_t
{
    // fiber is ready to be started
    READY,

    // fiber is in the process of pausing, to prevent undefined behaviour no
    // action should be taken on the fiber until it has transitioned to a
    // different state
    PAUSING,

    // fiber has been paused and is safe to resume
    RESUMABLE
};

}

