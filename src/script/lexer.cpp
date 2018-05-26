#include "lexer.hpp"

#include <cctype>
#include <iterator>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "error.hpp"
#include "location_data.hpp"
#include "script_exception.cpp"
#include "token.hpp"

namespace
{

/**
 * Simple helper struct to keep track of where we are in a string, as well the
 * equivalent location data in the source. This allows us to pass around a
 * single object which we can use to get the next character, as well as tell
 * if we run off the end of the string.
 */
struct string_cursor
{
    /** Iterator into source string. */
    std::string_view::const_iterator cursor;

    /** End iterator of source string. */
    std::string_view::const_iterator end;

    /** Location data about current position. */
    eng::location_data location;

    /**
     * Check if we have exhausted the string.
     *
     * @returns
     *   True if we are not at end of string, false otherwise.
     */
    bool is_valid() const
    {
        return cursor != end;
    }
};

/**
 * Peek at the next character in the script source.
 * Will throw an exception if cursor is at the end of the string.
 *
 * @param cursor
 *   Cursor into script source string.
 *
 * @returns
 *   Character currently at cursor.
 */
char peek(const string_cursor &cursor)
{
    if(cursor.cursor == cursor.end)
    {
        throw eng::script_exception({ cursor.location, eng::error_type::OUT_OF_SOURCE });
    }

    return *cursor.cursor;
}

/**
 * Return the current character at cursor, then advance to the next.
 * Will throw an exception if cursor is at the end of the string.
 *
 * @param cursor
 *   Cursor into script source string.
 *
 * @returns
 *   Character currently at cursor.
 */
char consume(string_cursor &cursor)
{
    const auto c = peek(cursor);
    ++cursor.cursor;

    return c;
}

/**
 * Check if a character is valid for an identifier token
 *
 * @param c
 *   Character to check.
 *
 * @returns
 *   True if char can be part of an identifier, false otherwise.
 */
bool is_identifier(const char c)
{
    return std::isalnum(c) || (c == '_');
}

/**
 * Lex an equality token from the cursor.
 *
 * @param c
 *   The first character of the equality token.
 *
 * @param cursor
 *   Cursor into script source string.
 *
 * @returns
 *   Equality token.
 */
eng::token lex_equality(
    const char c,
    string_cursor &cursor)
{
    // map of possible equality operators
    // as the lexed equality operator depends on the next character (e.g.
    // < or <=) the pair stored both possible options
    static const std::map<char, std::pair<eng::token_type, eng::token_type>> operators
    {
        { '=', { eng::token_type::EQUALS, eng::token_type::EQUAL_EQUAL } },
        { '<', { eng::token_type::LESS, eng::token_type::LESS_EQUAL } },
        { '>', { eng::token_type::MORE, eng::token_type::MORE_EQUAL } },
        { '!', { eng::token_type::BANG_EQUAL, eng::token_type::BANG_EQUAL } },
    };

    const auto &types = operators.at(c);

    // assume type is first element in the pair
    auto type = types.first;

    // if the next char is an equals sign then use the second element of the
    // type pair, also adjust the location data as we will be consuming an
    // additional token
    if(peek(cursor) == '=')
    {
        consume(cursor);
        ++cursor.location.offset;
        type = types.second;
    }

    return { type, cursor.location };
}

/**
 * Lex a string literal from the cursor.
 *
 * @param c
 *   The first character of the equality token.
 *
 * @param cursor
 *   Cursor into script source string.
 *
 * @returns
 *   String token.
 */
eng::token lex_string_literal(string_cursor &cursor)
{
    // store data about current position
    const auto lexeme_start = cursor;
    const auto start_line = cursor.location.line;

    // keep consuming until a we hit another single quote
    while(cursor.is_valid() && (peek(cursor) != '\''))
    {
        // as we support multi line strings then adjust line information when
        // we hit a newline character
        const auto current_char = consume(cursor);
        if(current_char == '\n')
        {
            ++cursor.location.line;
            cursor.location.offset = 0u;
        }
    }

    // consume final single quote
    consume(cursor);

    // construct lexeme, ignoring quote marks
    const auto lexeme = std::string(lexeme_start.cursor, cursor.cursor - 1u);

    // construct token
    const eng::token token {
        eng::token_type::STRING,
        lexeme,
        eng::location_data{ start_line, cursor.location.offset }
    };

    // adjust line offset
    cursor.location.offset += lexeme.length() - 1u;

    return token;
}

/**
 * Lex a number from the cursor.
 *
 * @param cursor
 *   Cursor into script source string.
 *
 * @returns
 *   Number token.
 */
eng::token lex_number(string_cursor &cursor)
{
    // store data about current position
    const auto lexeme_start = cursor;

    // keep consuming until we hit a non-digit character
    while(cursor.is_valid() && std::isdigit(peek(cursor)))
    {
        consume(cursor);
    }

    // if our next character is a full stop then consume it then keep consuming
    // more numbers (this is the decimal place character)
    if(cursor.is_valid() && (peek(cursor) == '.'))
    {
        consume(cursor);

        while(cursor.is_valid() && std::isdigit(peek(cursor)))
        {
            consume(cursor);
        }
    }

    // construct lexeme and token
    const std::string lexeme(lexeme_start.cursor - 1u, cursor.cursor);
    const eng::token token = { eng::token_type::NUMBER, lexeme, cursor.location };

    // adjust line offset
    cursor.location.offset += lexeme.length() - 1u;

    return token;
}

/**
 * Lex an identifier from the cursor, if the identifier represents a keyword
 * then a token representing that will be returned.
 *
 * @param cursor
 *   Cursor into script source string.
 *
 * @returns
 *   Identifier/keyword token.
 */
eng::token lex_identifier(string_cursor &cursor)
{
    // store data about current position
    const auto lexeme_start = cursor;

    // keep consuming until we hit a character which cannot be part of an
    // identifier
    while(cursor.is_valid() && is_identifier(peek(cursor)))
    {
        consume(cursor);
    }

    // construct lexeme
    const auto lexeme = std::string(lexeme_start.cursor - 1u, cursor.cursor);

    // map of keywords to associated token types
    static const std::map<std::string, eng::token_type> keywords {
        { "var", eng::token_type::VAR },
        { "if", eng::token_type::IF },
        { "while", eng::token_type::WHILE },
        { "do", eng::token_type::DO },
        { "break", eng::token_type::BREAK },
        { "for", eng::token_type::FOR },
        { "foreach", eng::token_type::FOREACH },
        { "true", eng::token_type::TRUE },
        { "false", eng::token_type::FALSE },
        { "in", eng::token_type::IN },
        { "function", eng::token_type::FUNCTION },
        { "return", eng::token_type::RETURN },
    };

    const auto keyword = keywords.find(lexeme);

    // if lexeme is a keyword then create a token for that keyword, else
    // create a identifier token
    const auto token = keyword != std::cend(keywords)
        ? eng::token{ keyword->second, lexeme, cursor.location }
        : eng::token{ eng::token_type::IDENTIFIER, lexeme, cursor.location };

    // adjust line offset
    cursor.location.offset += lexeme.length() - 1u;

    return token;
}

/**
 * Lex a token which is represents a single character in the script source.
 * As this is the 'base case' for lexing it will throw an exception if no
 * valid token can be found for the supplied character.
 *
 * @param c
 *   The character to lex into a token.
 *
 * @param cursor
 *   Cursor into script source string.
 *
 * @returns
 *  Token.
 */
eng::token lex_single_character_token(
    const char c,
    string_cursor &cursor)
{
    // map of char to token types
    static const std::map<char, eng::token_type> types
    {
        { '+' , eng::token_type::PLUS },
        { '-' , eng::token_type::MINUS },
        { '*' , eng::token_type::STAR },
        { '/' , eng::token_type::SLASH },
        { '[' , eng::token_type::LEFT_SQUARE },
        { ']' , eng::token_type::RIGHT_SQUARE },
        { '{' , eng::token_type::LEFT_BRACE },
        { '}' , eng::token_type::RIGHT_BRACE },
        { '(' , eng::token_type::LEFT_PAREN },
        { ')' , eng::token_type::RIGHT_PAREN },
        { ',' , eng::token_type::COMMA },
        { ';' , eng::token_type::SEMICOLON },
        { ':' , eng::token_type::COLON }
    };

    const auto type = types.find(c);

    // throw an exception if no token type can be found for the supplied
    // character
    if(type == std::cend(types))
    {
        throw eng::script_exception({
            eng::location_data{ cursor.location },
            eng::error_type::UNKNOWN_TOKEN });
    }

    return { type->second, cursor.location };
}

}

namespace eng
{

lexer::lexer(std::string_view source)
    : tokens_()
{
    std::vector<error> errors{ };

    string_cursor cursor{
        std::cbegin(source),
        std::cend(source),
        { 1u, 0u }
    };

    // keep lexing until we exhaust the supplied source string
    while(cursor.is_valid())
    {
        const auto c = consume(cursor);

        try
        {
            switch(c)
            {
                case '\n':
                    // if we encounter a new line then adjust location data and
                    // continue to next character
                    ++cursor.location.line;
                    cursor.location.offset = 0u;
                    continue;
                case ' ':  // deliberate fall-through
                case '\t': // deliberate fall-through
                case '\r': // deliberate fall-through
                    // ignore white space
                    break;
                case '=': // deliberate fall-through
                case '<': // deliberate fall-through
                case '>': // deliberate fall-through
                case '!':
                    tokens_.emplace_back(lex_equality(c, cursor));
                    break;
                case '/':
                    // if we have double slash then we have a comment
                    if(peek(cursor) == '/')
                    {
                        // consume all characters until we reach the end of the
                        // line, this effectively discards them
                        while(cursor.is_valid() && (peek(cursor) != '\n'))
                        {
                            consume(cursor);
                        }
                    }
                    else
                    {
                        // lex single slash as a single character token
                        tokens_.emplace_back(lex_single_character_token(c, cursor));
                    }
                    break;
                case '\'':
                    tokens_.emplace_back(lex_string_literal(cursor));
                    break;
                default:
                {
                    if(std::isdigit(c))
                    {
                        tokens_.emplace_back(lex_number(cursor));
                    }
                    else if(is_identifier(c))
                    {
                        tokens_.emplace_back(lex_identifier(cursor));
                    }
                    else
                    {
                        tokens_.emplace_back(lex_single_character_token(c, cursor));
                    }
                    break;
                }
            }
        }
        catch(script_exception &e)
        {
            // rather than allowing the exception to escape we take a copy of
            // the errors and continue, this allows us a chance to report as
            // many errors as possible
            const auto &caught_errors = e.errors();
            errors.insert(
                std::cend(errors),
                std::begin(caught_errors),
                std::end(caught_errors));
        }

        ++cursor.location.offset;
    }

    // add sentinel token, this makes the following stages simpler as it is
    // easier to determine when we have exhausted all the tokens
    tokens_.emplace_back(
        token_type::END,
        eng::location_data{ cursor.location.line + 1u, 0u });

    // if we encountered errors then propagate them up
    if(!errors.empty())
    {
        throw script_exception(errors);
    }
}

std::vector<token> lexer::tokens() const
{
    return tokens_;
}

}

