#include <algorithm>
#include <cstddef>
#include <vector>

#include <gtest/gtest.h>

#include "error.hpp"
#include "expression.hpp"
#include "lexer.hpp"
#include "location_data.hpp"
#include "parser.hpp"
#include "script_exception.hpp"
#include "statement.hpp"

namespace
{

void check_errors(
    const std::string &source,
    const std::vector<eng::error> &expected,
    const std::size_t error_limit=0u)
{
    auto threw = false;
    eng::lexer l{ source };

    try
    {
        eng::parser p{ l.tokens() };
    }
    catch(const eng::script_exception &e)
    {
        threw = true;

        if(error_limit == 0u)
        {
            EXPECT_EQ(expected, e.errors());
        }
        else
        {
            for(auto i = 0u; i < std::min(e.errors().size(), error_limit); ++i)
            {
                EXPECT_EQ(expected[i], e.errors()[i]);
            }
        }
    }

    EXPECT_TRUE(threw);
}

}

TEST(parser_error_tests, missing_semicolon_number_statement)
{
    check_errors(
        R"src(0.1)src",
        { { eng::location_data{  1u, 3u }, eng::error_type::MISSING_SEMICOLON } });
}

TEST(parser_error_tests, missing_semicolon_string_statement)
{
    check_errors(
        R"src('hello')src",
        { { eng::location_data{  1u, 7u }, eng::error_type::MISSING_SEMICOLON } });
}

TEST(parser_error_tests, missing_semicolon_bool_statement)
{
   check_errors(
       R"src(true)src",
       { { eng::location_data{  1u, 4u }, eng::error_type::MISSING_SEMICOLON } });
}

TEST(parser_error_tests, missing_semicolon_multiple)
{
    check_errors(
        R"src(
111
111;
'yo'
'yo';
true
)src",
    {
        { eng::location_data{ 2u, 3u }, eng::error_type::MISSING_SEMICOLON },
        { eng::location_data{ 4u, 4u }, eng::error_type::MISSING_SEMICOLON },
        { eng::location_data{ 6u, 4u }, eng::error_type::MISSING_SEMICOLON }
    });
}

TEST(parser_error_tests, missing_semicolon_variable_declaration)
{
    check_errors(
        R"src(var a = 'hello')src",
        { { eng::location_data{  1u, 15u }, eng::error_type::MISSING_SEMICOLON } });
}

TEST(parser_error_tests, missing_equals_variable_declaration)
{
    check_errors(
        R"src(var a 'hello';)src",
        { { eng::location_data{  1u, 5u }, eng::error_type::MISSING_EQUALS } });
}

TEST(parser_error_tests, expected_identifier_variable_declaration)
{
    check_errors(
        R"src(var true = 'hello';)src",
        { { eng::location_data{  1u, 3u }, eng::error_type::EXPECTED_IDENTIFIER } });
}

TEST(parser_error_tests, missing_semicolon_break)
{
    check_errors(
        R"src(break)src",
        { { eng::location_data{  1u, 5u }, eng::error_type::MISSING_SEMICOLON } });
}

TEST(parser_error_tests, missing_right_brace_block)
{
    check_errors(
        R"src({var a = 1;)src",
        { { eng::location_data{  1u, 11u }, eng::error_type::MISSING_RIGHT_BRACE } });
}

TEST(parser_error_tests, missing_colon_object)
{
    check_errors(
        R"src(var a = { 0 false };)src",
        { { eng::location_data{  1u, 11u }, eng::error_type::MISSING_COLON } });
}

TEST(parser_error_tests, missing_right_square_assignment)
{
    check_errors(
        R"src(a[1 = 3;)src",
        { { eng::location_data{  1u, 7u }, eng::error_type::INVALID_ASSIGNMENT } });
}

TEST(parser_error_tests, missing_right_square_expression)
{
    check_errors(
        R"src(3+a[1;)src",
        { { eng::location_data{  1u, 5u }, eng::error_type::MISSING_RIGHT_SQUARE } });
}

TEST(parser_error_tests, missing_right_paren_expression)
{
    check_errors(
        R"src((a)src",
        { { eng::location_data{  1u, 2u }, eng::error_type::MISSING_RIGHT_PAREN } });
}

TEST(parser_error_tests, missing_left_paren_if)
{
    check_errors(
        R"src(if a==1){})src",
        { { eng::location_data{  1u, 2u }, eng::error_type::MISSING_LEFT_PAREN } });
}

TEST(parser_error_tests, missing_right_paren_if)
{
    check_errors(
        R"src(if(a == 1 {})src",
        { { eng::location_data{  1u, 9u }, eng::error_type::MISSING_RIGHT_PAREN } });
}

TEST(parser_error_tests, missing_left_paren_while)
{
    check_errors(
        R"src(while a==1){})src",
        { { eng::location_data{  1u, 5u }, eng::error_type::MISSING_LEFT_PAREN } });
}

TEST(parser_error_tests, missing_right_paren_while)
{
    check_errors(
        R"src(while(a == 1 {})src",
        { { eng::location_data{  1u, 12u }, eng::error_type::MISSING_RIGHT_PAREN } });
}

TEST(parser_error_tests, missing_while_do_while)
{
    check_errors(
        R"src(do {} (a == 1))src",
        { { eng::location_data{  1u, 5u }, eng::error_type::MISSING_WHILE } });
}

TEST(parser_error_tests, missing_left_paren_do_while)
{
    check_errors(
        R"src(do {} while a == 1))src",
        { { eng::location_data{  1u, 11u }, eng::error_type::MISSING_LEFT_PAREN } });
}

TEST(parser_error_tests, missing_right_paren_do_while)
{
    check_errors(
        R"src(do {} while(a == 1)src",
        { { eng::location_data{  1u, 18u }, eng::error_type::MISSING_RIGHT_PAREN } });
}

TEST(parser_error_tests, missing_semi_colon_for)
{
    check_errors(
        R"src(for(var i = 0; i < 1 i = i + 1){})src",
        { { eng::location_data{  1u, 20u }, eng::error_type::MISSING_SEMICOLON } });
}

TEST(parser_error_tests, missing_left_paren_for)
{
    check_errors(
        R"src(for var i = 0; i < 1; i = i + 1){})src",
        { { eng::location_data{  1u, 3u }, eng::error_type::MISSING_LEFT_PAREN } },
        1u);
}

TEST(parser_error_tests, missing_right_paren_for)
{
    check_errors(
        R"src(for(var i = 0; i < 1; i = i + 1 {})src",
        { { eng::location_data{  1u, 31u }, eng::error_type::MISSING_RIGHT_PAREN } });
}

TEST(parser_error_tests, missing_var_for_each)
{
    check_errors(
        R"src(foreach(a in b){})src",
        { { eng::location_data{  1u, 8u }, eng::error_type::MISSING_VAR } });
}

TEST(parser_error_tests, missing_in_for_each)
{
    check_errors(
        R"src(foreach(var a b){ })src",
        { { eng::location_data{  1u, 13u }, eng::error_type::MISSING_IN } });
}

TEST(parser_error_tests, unexpected_symbol)
{
    check_errors(
        R"src(*5;)src",
        { { eng::location_data{  1u, 1u }, eng::error_type::UNEXPECTED_SYMBOL } });
}

TEST(parser_error_tests, missing_left_paren_for_each)
{
    check_errors(
        R"src(foreach var a in b){ })src",
        { { eng::location_data{  1u, 7u }, eng::error_type::MISSING_LEFT_PAREN } },
        1u);
}

TEST(parser_error_tests, missing_right_paren_for_each)
{
    check_errors(
        R"src(foreach(var a in b { })src",
        { { eng::location_data{  1u, 18u }, eng::error_type::MISSING_RIGHT_PAREN } });
}

TEST(parser_error_tests, unexpected_end)
{
    check_errors(
        R"src(3 +)src",
        { { eng::location_data{  1u, 3u }, eng::error_type::UNEXPECTED_END } });
}

TEST(parser_error_tests, missing_left_brace)
{
    check_errors(
        R"src(if(a == 1))src",
        { { eng::location_data{  1u, 10u }, eng::error_type::MISSING_LEFT_BRACE } });
}

TEST(parser_error_tests, function_missing_identfifier)
{
    check_errors(
        R"src(function (){ })src",
        { { eng::location_data{  1u, 8u }, eng::error_type::EXPECTED_IDENTIFIER } });
}

TEST(parser_error_tests, function_missing_left_paran)
{
    check_errors(
        R"src(function foo){ })src",
        { { eng::location_data{  1u, 12u }, eng::error_type::MISSING_LEFT_PAREN } });
}

TEST(parser_error_tests, function_missing_var)
{
    check_errors(
        R"src(function foo(a){ })src",
        { { eng::location_data{  1u, 13u }, eng::error_type::MISSING_VAR } });
}

TEST(parser_error_tests, function_missing_comma)
{
    check_errors(
        R"src(function foo(var a var b){ })src",
        { { eng::location_data{  1u, 18u }, eng::error_type::MISSING_COMMA } });
}

TEST(parser_error_tests, call_missing_right_comma)
{
    check_errors(
        R"src(a(1;)src",
        { { eng::location_data{  1u, 3u }, eng::error_type::MISSING_COMMA } });
}

