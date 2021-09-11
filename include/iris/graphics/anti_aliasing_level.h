#pragma once

#include <cstdint>

namespace iris
{

enum class AntiAliasingLevel : std::uint8_t
{
    NONE = 0,
    LOW = 2,
    MEDIUM = 4,
    HIGH = 6
};

}
