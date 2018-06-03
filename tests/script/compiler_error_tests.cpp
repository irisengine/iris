#include <algorithm>
#include <cstddef>
#include <vector>

#include <gtest/gtest.h>

#include "compiler.hpp"
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
    eng::parser p{ l.tokens() };

    try
    {
        eng::compiler c{ p.yield() };
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

TEST(compiler_error_tests, undeclared_variable)
{
    check_errors(
        R"src(a;)src",
        { { eng::location_data{  1u, 1u }, eng::error_type::UNDECLARED_VARIABLE } });
}


TEST(compiler_error_tests, undeclared_assignment)
{
    check_errors(
        R"src(a = 1;)src",
        { { eng::location_data{ 1u, 1u }, eng::error_type::UNDECLARED_VARIABLE } });
}


TEST(compiler_error_tests, undeclared_get)
{
    check_errors(
        R"src(a[0];)src",
        { { eng::location_data{ 1u, 1u }, eng::error_type::UNDECLARED_VARIABLE } });
}


TEST(compiler_error_tests, undeclared_set)
{
    check_errors(
        R"src(a[0] = 1;)src",
        { { eng::location_data{ 1u, 1u }, eng::error_type::UNDECLARED_VARIABLE } });
}

TEST(compiler_error_tests, break_outside_conditional)
{
    check_errors(
        R"src(break;)src",
        { { eng::location_data{ 1u, 5u }, eng::error_type::BREAK_SCOPE } });
}

TEST(compiler_error_tests, use_variable_outside_of_scope)
{
    check_errors(
        R"src({var a = 0; } a;)src",
        { { eng::location_data{ 1u, 15u }, eng::error_type::UNDECLARED_VARIABLE } });
}

