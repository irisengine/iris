#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <variant>

#include "location_data.hpp"
#include "token_type.hpp"
#include "value.hpp"

namespace eng
{

/**
 * A token is the smallest collection of characters that have syntatic meaning
 * in a program. This struct encapsulates that collection of characters (lexeme)
 * and other supporting data.
 */
struct token
{
    /**
     * Construct a new token.
     *
     * This constructor assumes the lexeme is the same as the token i.e. a
     * keyword or symbol.
     *
     * @param type
     *   The token type.
     *
     * @param location
     *   Location of token in source.
     */
    token(
        const token_type type,
        const location_data location)
        : token(type, { }, location)
    {
        // set the lexeme to be the token type
        std::stringstream strm{ };
        strm << type;
        lexeme = strm.str();
    }

    /**
     * Construct a new token.
     *
     * @param type
     *   The token type.
     *
     * @param lexeme
     *   Explicit lexeme value.
     *
     * @param location
     *   Location of token in source.
     */
    token(
        const token_type type,
        const std::string &lexeme,
        const location_data location)
        : type(type),
          lexeme(lexeme),
          val(0.0f),
          location(location)
    {
        // set values based on token type
        switch(type)
        {
            case token_type::STRING:
                // add quotes as that is how they will have appeared in source,
                // this ensures offset are correct (e.g. for error reporting)
                this->lexeme = "'" + lexeme + "'";
                val = value{ lexeme };
                break;
            case token_type::NUMBER:
                val = value{ std::stof(lexeme, nullptr) };
                break;
            case token_type::TRUE:
                val = value(true);
                break;
            case token_type::FALSE:
                val = value(false);
                break;
            default:
                break;
        }
    }

    // default
    ~token() = default;
    token(const token&) = default;
    token& operator=(const token&) = default;
    token(token&&) = default;
    token& operator=(token&&) = default;

    /**
     * Equality operator, checks if both tokens are the same.
     *
     * @param other
     *   Token to compare with.
     *
     * @returns True if both tokens are the same, false otherwise.
     */
    bool operator==(const token &other) const
    {
        return
            (type == other.type) &&
            (lexeme == other.lexeme) &&
            (val == other.val) &&
            (location == other.location);
    }

    /**
     * Inequality operator, checks if both tokens are the not same.
     *
     * @param other
     *   Token to compare with.
     *
     * @returns True if both tokens are not the same, false otherwise.
     */
    bool operator!=(const token &other) const
    {
        return !(*this == other);
    }

    /**
     * Write token to a stream, useful for debugging.
     *
     * @param out
     *   Stream to write to.
     *
     * @param t
     *   token to write to stream.
     *
     * @returns
     *   Reference to input stream.
     */
    friend std::ostream& operator<<(std::ostream &out, const token &t)
    {
        out << t.type << " " << t.lexeme << " ";
        out << t.val;
        out << " " << t.location.line << ":" << t.location.offset;

        return out;
    }

    /** Type of token. */
    token_type type;

    /**
     * Token lexeme. Usually the name of the token unless it is a user controlled
     * value, e.g. a variable name
     */
    std::string lexeme;

    /**
     * Value of token, will be a default value if token is for a keyword or
     * symbol.
     */
    value val;

    /** Location of token in source. */
    location_data location;
};

}

