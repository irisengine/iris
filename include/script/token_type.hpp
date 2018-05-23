#pragma once

#include <cstdint>
#include <iostream>
#include <sstream>

namespace eng
{

/**
 * Enumeration of all supported token types.
 */
enum class token_type : std::uint32_t
{
    VAR,
    IDENTIFIER,
    STRING,
    NUMBER,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    EQUAL_EQUAL,
    BANG_EQUAL,
    LESS,
    LESS_EQUAL,
    MORE,
    MORE_EQUAL,
    IF,
    WHILE,
    DO,
    BREAK,
    FOR,
    FOREACH,
    IN,
    TRUE,
    FALSE,
    LEFT_PAREN,
    RIGHT_PAREN,
    EQUALS,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_SQUARE,
    RIGHT_SQUARE,
    COLON,
    COMMA,
    SEMICOLON,
    FUNCTION,
    RETURN,
    END
};

/**
 * Write token_type to a stream.
 *
 * @param out
 *   Stream to write to.
 *
 * @param type
 *   token_type to write to stream.
 *
 * @returns
 *   Reference to input stream.
 */
inline std::ostream& operator<<(std::ostream &out, const token_type type)
{
    switch(type)
    {
        case token_type::VAR:
            out << "var";
            break;
        case token_type::IDENTIFIER:
            out << "identifier";
            break;
        case token_type::STRING:
            out << "string";
            break;
        case token_type::NUMBER:
            out << "number";
            break;
        case token_type::PLUS:
            out << "+";
            break;
        case token_type::MINUS:
            out << "-";
            break;
        case token_type::STAR:
            out << "*";
            break;
        case token_type::SLASH:
            out << "/";
            break;
        case token_type::EQUAL_EQUAL:
            out << "==";
            break;
        case token_type::BANG_EQUAL:
            out << "!=";
            break;
        case token_type::LESS:
            out << "<";
            break;
        case token_type::LESS_EQUAL:
            out << "<=";
            break;
        case token_type::MORE:
            out << ">";
            break;
        case token_type::MORE_EQUAL:
            out << ">=";
            break;
        case token_type::IF:
            out << "if";
            break;
        case token_type::WHILE:
            out << "while";
            break;
        case token_type::DO:
            out << "do";
            break;
        case token_type::BREAK:
            out << "break";
            break;
        case token_type::FOR:
            out << "for";
            break;
        case token_type::FOREACH:
            out << "foreach";
            break;
        case token_type::IN:
            out << "in";
            break;
        case token_type::EQUALS:
            out << "=";
            break;
        case token_type::TRUE:
            out << "true";
            break;
        case token_type::FALSE:
            out << "false";
            break;
        case token_type::LEFT_PAREN:
            out << "(";
            break;
        case token_type::RIGHT_PAREN:
            out << ")";
            break;
        case token_type::LEFT_BRACE:
            out << "{";
            break;
        case token_type::RIGHT_BRACE:
            out << "}";
            break;
        case token_type::LEFT_SQUARE:
            out << "[";
            break;
        case token_type::RIGHT_SQUARE:
            out << "]";
            break;
        case token_type::COLON:
            out << ":";
            break;
        case token_type::COMMA:
            out << ",";
            break;
        case token_type::SEMICOLON:
            out << ";";
            break;
        case token_type::FUNCTION:
            out << "function";
            break;
        case token_type::RETURN:
            out << "return";
            break;
        case token_type::END:
            out << "end";
            break;
    }

    return out;
}

}


