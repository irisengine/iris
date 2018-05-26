#include <iterator>
#include <vector>

#include <gtest/gtest.h>

#include "error.hpp"
#include "lexer.hpp"
#include "location_data.hpp"
#include "script_exception.hpp"
#include "token.hpp"

TEST(lexer_tests, valid_token_types)
{
    const auto source = R"src(
= < <= > >= == != + - * / ; : { } [ ] ( ) ,
)src";

    const eng::lexer l{ source };
    const auto tokens = l.tokens();
    std::vector<eng::token_type> actual_types{ };
    std::transform(
        std::cbegin(tokens),
        std::cend(tokens),
        std::back_inserter(actual_types),
        [](const eng::token &token) { return token.type; });

    const std::vector<eng::token_type> expected{
        eng::token_type::EQUALS,
        eng::token_type::LESS,
        eng::token_type::LESS_EQUAL,
        eng::token_type::MORE,
        eng::token_type::MORE_EQUAL,
        eng::token_type::EQUAL_EQUAL,
        eng::token_type::BANG_EQUAL,
        eng::token_type::PLUS,
        eng::token_type::MINUS,
        eng::token_type::STAR,
        eng::token_type::SLASH,
        eng::token_type::SEMICOLON,
        eng::token_type::COLON,
        eng::token_type::LEFT_BRACE,
        eng::token_type::RIGHT_BRACE,
        eng::token_type::LEFT_SQUARE,
        eng::token_type::RIGHT_SQUARE,
        eng::token_type::LEFT_PAREN,
        eng::token_type::RIGHT_PAREN,
        eng::token_type::COMMA,
        eng::token_type::END
    };

    ASSERT_EQ(expected, actual_types);
}

TEST(lexer_tests, valid_keywords)
{
    const auto source = R"src(
var if while do break for foreach true false in function return
)src";

    const eng::lexer l{ source };
    const auto tokens = l.tokens();
    std::vector<eng::token_type> actual_types{ };
    std::transform(
        std::cbegin(tokens),
        std::cend(tokens),
        std::back_inserter(actual_types),
        [](const eng::token &token) { return token.type; });

    const std::vector<eng::token_type> expected{
        eng::token_type::VAR,
        eng::token_type::IF,
        eng::token_type::WHILE,
        eng::token_type::DO,
        eng::token_type::BREAK,
        eng::token_type::FOR,
        eng::token_type::FOREACH,
        eng::token_type::TRUE,
        eng::token_type::FALSE,
        eng::token_type::IN,
        eng::token_type::FUNCTION,
        eng::token_type::RETURN,
        eng::token_type::END
    };

    ASSERT_EQ(expected, actual_types);
}

TEST(lexer_tests, single_line_program)
{
    const auto source = R"src(var a = 10.0;)src";

    const eng::lexer l{ source };
    const auto tokens = l.tokens();

    const std::vector<eng::token> expected{
        eng::token(eng::token_type::VAR, { 1u, 0u }),
        eng::token(eng::token_type::IDENTIFIER, "a", { 1u, 4u }),
        eng::token(eng::token_type::EQUALS, { 1u, 6u }),
        eng::token(eng::token_type::NUMBER, "10.0", { 1u, 8u }),
        eng::token(eng::token_type::SEMICOLON, { 1u, 12u }),
        eng::token(eng::token_type::END, { 2u, 0u })
    };

    ASSERT_EQ(expected, l.tokens());
}

TEST(lexer_tests, variable_name_with_numbers)
{
    const auto source = R"src(var a2 = 10.0;)src";

    const eng::lexer l{ source };
    const auto tokens = l.tokens();

    const std::vector<eng::token> expected{
        eng::token(eng::token_type::VAR, { 1u, 0u }),
        eng::token(eng::token_type::IDENTIFIER, "a2", { 1u, 4u }),
        eng::token(eng::token_type::EQUALS, { 1u, 7u }),
        eng::token(eng::token_type::NUMBER, "10.0", { 1u, 9u }),
        eng::token(eng::token_type::SEMICOLON, { 1u, 13u }),
        eng::token(eng::token_type::END, { 2u, 0u })
    };

    ASSERT_EQ(expected, l.tokens());
}

TEST(lexer_tests, multi_line_program)
{
    const auto source = R"src(
var a = 10.0;
var
b
=
true
;
)src";

    const eng::lexer l{ source };
    const auto tokens = l.tokens();

    const std::vector<eng::token> expected{
        eng::token(eng::token_type::VAR, { 2u, 0u }),
        eng::token(eng::token_type::IDENTIFIER, "a", { 2u, 4u }),
        eng::token(eng::token_type::EQUALS, { 2u, 6u }),
        eng::token(eng::token_type::NUMBER, "10.0", { 2u, 8u }),
        eng::token(eng::token_type::SEMICOLON, { 2u, 12u }),
        eng::token(eng::token_type::VAR, { 3u, 0u }),
        eng::token(eng::token_type::IDENTIFIER, "b", { 4u, 0u }),
        eng::token(eng::token_type::EQUALS, { 5u, 0u }),
        eng::token(eng::token_type::TRUE, { 6u, 0u }),
        eng::token(eng::token_type::SEMICOLON, { 7u, 0u }),
        eng::token(eng::token_type::END, { 9u, 0u })
    };

    ASSERT_EQ(expected, l.tokens());
}

TEST(lexer_tests, comments_ignored)
{
    const auto source = R"src(// this is a comment
var a = 10.0;
// so is this
var aa = true;
)src";

    const eng::lexer l{ source };
    const auto tokens = l.tokens();

    const std::vector<eng::token> expected{
        eng::token(eng::token_type::VAR, { 2u, 0u }),
        eng::token(eng::token_type::IDENTIFIER, "a", { 2u, 4u }),
        eng::token(eng::token_type::EQUALS, { 2u, 6u }),
        eng::token(eng::token_type::NUMBER, "10.0", { 2u, 8u }),
        eng::token(eng::token_type::SEMICOLON, { 2u, 12u }),
        eng::token(eng::token_type::VAR, { 4u, 0u }),
        eng::token(eng::token_type::IDENTIFIER, "aa", { 4u, 4u }),
        eng::token(eng::token_type::EQUALS, { 4u, 7u }),
        eng::token(eng::token_type::TRUE, { 4u, 9u }),
        eng::token(eng::token_type::SEMICOLON, { 4u, 13u }),
        eng::token(eng::token_type::END, { 6u, 0u })
    };

    ASSERT_EQ(expected, l.tokens());
}

TEST(lexer_tests, single_line_string)
{
    const auto source = R"src('hello')src";

    const eng::lexer l{ source };
    const auto tokens = l.tokens();

    const std::vector<eng::token> expected{
        eng::token(eng::token_type::STRING, "hello", { 1u, 0u }),
        eng::token(eng::token_type::END, { 2u, 0u })
    };

    ASSERT_EQ(expected, l.tokens());
}

TEST(lexer_tests, multi_line_string)
{
    const auto source = R"src('hello
world')src";

    const eng::lexer l{ source };
    const auto tokens = l.tokens();

    const std::vector<eng::token> expected{
        eng::token(eng::token_type::STRING, "hello\nworld", { 1u, 0u }),
        eng::token(eng::token_type::END, { 3u, 0u })
    };

    ASSERT_EQ(expected, l.tokens());
}

TEST(lexer_tests, invalid_character)
{
    const auto source = R"src(@)src";
    std::vector<eng::error> errors{ };

    try
    {
        eng::lexer{ source };
    }
    catch(eng::script_exception &e)
    {
        errors = e.errors();
    }

    const std::vector<eng::error> expected{
        eng::error( eng::location_data{ 1u, 0u }, eng::error_type::UNKNOWN_TOKEN)
    };

    ASSERT_EQ(expected, errors);
}

TEST(lexer_tests, multiple_invalid_character)
{
    const auto source = R"src(
@
var a = 1;
a;~)src";
    std::vector<eng::error> errors{ };

    try
    {
        eng::lexer{ source };
    }
    catch(eng::script_exception &e)
    {
        errors = e.errors();
    }

    const std::vector<eng::error> expected{
        eng::error( eng::location_data{ 2u, 0u }, eng::error_type::UNKNOWN_TOKEN),
        eng::error( eng::location_data{ 4u, 2u }, eng::error_type::UNKNOWN_TOKEN)
    };

    ASSERT_EQ(expected, errors);
}

