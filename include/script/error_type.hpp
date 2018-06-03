#pragma once

#include <cstdint>

namespace eng
{

/**
 * Enumeration of possible script error types.
 */
enum class error_type : std::uint32_t
{
    // lexer errors
    UNKNOWN_TOKEN,
    OUT_OF_SOURCE,

    // parser errors
    UNEXPECTED_SYMBOL,
    UNEXPECTED_END,
    EXPECTED_IDENTIFIER,
    INVALID_ASSIGNMENT,
    MISSING_SEMICOLON,
    MISSING_COMMA,
    MISSING_EQUALS,
    MISSING_LEFT_BRACE,
    MISSING_RIGHT_BRACE,
    MISSING_RIGHT_SQUARE,
    MISSING_LEFT_PAREN,
    MISSING_RIGHT_PAREN,
    MISSING_COLON,
    MISSING_WHILE,
    MISSING_VAR,
    MISSING_IN,

    // compiler errors
    UNDECLARED_VARIABLE,
    BREAK_SCOPE,

    // virtual machine errors
    RUNTIME_ERROR,
    INCORRECT_ARG_NUM,
    ITERATOR_IMBALANCE,
    STACK_IMBALANCE,
    UNKNOWN_OPCODE
};

}

