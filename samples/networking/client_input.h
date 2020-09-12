#pragma once

#include "core/vector3.h"
#include "networking/data_buffer_serialiser.h"
#include "networking/data_buffer_deserialiser.h"
#include <cstdint>

/**
 * Struct encapsulating user input.
 */
struct ClientInput
{
    /**
     * Create new ClientInput.
     */
    ClientInput()
        : forward(0.0f),
          side(0.0f),
          tick(0u)
    { }

    /**
     * Create new ClientInput with a deserialiser.
     * 
     * @param deserialiser
     *   Deserialiser object.
     */
    ClientInput(iris::DataBufferDeserialiser &deserialiser)
        : forward(deserialiser.pop<float>()),
          side(deserialiser.pop<float>()),
          tick(deserialiser.pop<std::uint32_t>())
    { }

    /**
     * Serialise object.
     * 
     * @param serialiser.
     *   Serialiser object.
     */
    void serialise(iris::DataBufferSerialiser &serialiser) const
    {
        serialiser.push(forward);
        serialiser.push(side);
        serialiser.push(tick);
    }

    /** Relative amount user is moving forward, should be in range [-1.0, 1.0]. */
    float forward;

    /** Relative amount user is moving sideways, should be in range [-1.0, 1.0]. */
    float side;

    /** Client tick input is for. */
    std::uint32_t tick;
};

