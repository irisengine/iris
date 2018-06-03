#pragma once

#include <cstdint>

namespace eng
{

/**
 * Enumeration of possible expression types.
 */
enum class expression_type : std::uint8_t
{
    LITERAL,
    VARIABLE,
    ASSIGNMENT,
    OBJECT,
    BINOP,
    GET,
    SET,
    CALL
};

}

