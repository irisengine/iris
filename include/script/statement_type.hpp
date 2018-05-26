#pragma once

#include <cstdint>

namespace eng
{

/**
 * Enumeration of possible statement types.
 */
enum class statement_type : std::uint8_t
{
    VARIABLE,
    EXPRESSION,
    BLOCK,
    IF,
    WHILE,
    DO_WHILE,
    FOR,
    FOR_EACH,
    FUNCTION,
    RETURN,
    BREAK
};

}


