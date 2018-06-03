#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "compiler.hpp"
#include "lexer.hpp"
#include "opcode.hpp"
#include "parser.hpp"
#include "value.hpp"

using std::string_literals::operator""s;

namespace
{

void check_output(
    const std::string &source,
    const std::vector<std::uint8_t> &opcodes,
    const std::vector<eng::value> &constants,
    const std::vector<std::string> &globals,
    const std::uint8_t num_objects=0u)
{
    eng::lexer l{ source };
    eng::parser p{ l.tokens() };
    eng::compiler c{ p.yield() };

    const auto chunk = c.get_chunk();
    std::cout << chunk.variables().top().size() << std::endl;
    const auto &vars = chunk.variables().top();
    std::cout << vars.size() << std::endl;

    for(const auto &v : vars)
    {
        std::cout << v.first << std::endl;
        std::cout << v.second << std::endl;
    }

    EXPECT_EQ(opcodes, chunk.code());
    EXPECT_EQ(constants, chunk.constants());
    EXPECT_EQ(num_objects, chunk.num_objects());
    EXPECT_EQ(globals.size(), vars.size());

    for(const auto &var : globals)
    {
        EXPECT_NE(std::cend(vars), vars.find(var));
    }
}

}

TEST(compiler_tests, empty_program_stop)
{
    check_output("", { eng::assemble(eng::opcode::STOP) }, {}, {}, {});
}

TEST(compiler_tests, single_expression_statement)
{
    check_output(
         R"src(1.5;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::POP),
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.5f }
        },
        {});
}

TEST(compiler_tests, single_variable_statement)
{
    check_output(
        R"src(var a = 1.5;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.5f }
        },
        {
            "a"
        });
}

TEST(compiler_tests, multi_variable_statement)
{
    check_output(
        R"src(
var a = 1.5;
var b = 'hello';
var c = 1.5;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            1u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            2u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.5f },
            eng::value{ "hello"s }
        },
        {
            "a",
            "b",
            "c"
        });
}

TEST(compiler_tests, variable_reassignment)
{
    check_output(
        R"src(
var a = 1.5;
a = 3;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.5f },
            eng::value{ 3.0f },
        },
        {
            "a",
        });
}

TEST(compiler_tests, empty_object_statement)
{
    check_output(
        R"src(var a = { };)src",
        {
            eng::assemble(eng::opcode::PUSH_OBJECT),
            0u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        { },
        {
            "a",
        },
        1u);
}

TEST(compiler_tests, object_statement)
{
    check_output(
        R"src(var a = { 0 : 'hello', 1 : 'yo' };)src",
        {
            eng::assemble(eng::opcode::PUSH_OBJECT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::SET_OBJECT),
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            2u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            3u,
            eng::assemble(eng::opcode::SET_OBJECT),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ "hello"s },
            eng::value{ 0.0f },
            eng::value{ "yo"s },
            eng::value{ 1.0f },
        },
        {
            "a",
        },
        1u);
}

TEST(compiler_tests, addition_expression)
{
    check_output(
        R"src(var a = 1 + 2;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::ADD),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 2.0f },
        },
        {
            "a",
        });
}

TEST(compiler_tests, subtraction_expression)
{
    check_output(
        R"src(var a = 1 - 2;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::SUB),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 2.0f },
        },
        {
            "a",
        });
}

TEST(compiler_tests, multiplication_expression)
{
    check_output(
        R"src(var a = 1 * 2;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::MUL),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 2.0f },
        },
        {
            "a",
        });
}

TEST(compiler_tests, division_expression)
{
    check_output(
        R"src(var a = 1 / 2;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::DIV),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 2.0f },
        },
        {
            "a",
        });
}

TEST(compiler_tests, compare_equality_expression)
{
    check_output(
        R"src(var a = 1 == 2;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::CMP_EQ),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 2.0f },
        },
        {
            "a",
        });
}

TEST(compiler_tests, compare_not_equality_expression)
{
    check_output(
        R"src(var a = 1 != 2;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::CMP_NEQ),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 2.0f },
        },
        {
            "a",
        });
}

TEST(compiler_tests, compare_less_expression)
{
    check_output(
        R"src(var a = 1 < 2;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::CMP_LESS),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 2.0f },
        },
        {
            "a",
        });
}

TEST(compiler_tests, compare_less_equal_expression)
{
    check_output(
        R"src(var a = 1 <= 2;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::CMP_LESS_EQ),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 2.0f },
        },
        {
            "a",
        });
}

TEST(compiler_tests, compare_more_expression)
{
    check_output(
        R"src(var a = 1 > 2;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::CMP_MORE),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 2.0f },
        },
        {
            "a",
        });
}

TEST(compiler_tests, compare_more_equal_expression)
{
    check_output(
        R"src(var a = 1 >= 2;)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::CMP_MORE_EQ),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 2.0f },
        },
        {
            "a",
        });
}

TEST(compiler_tests, get_expression)
{
    check_output(
        R"src(var a = {0 : 1}; a[0];)src",
        {
            eng::assemble(eng::opcode::PUSH_OBJECT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::SET_OBJECT),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::PUSH_VARIABLE),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::GET_OBJECT),
            eng::assemble(eng::opcode::POP),
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 0.0f },
        },
        {
            "a",
        },
        1u);
}

TEST(compiler_tests, set_expression)
{
    check_output(
        R"src(var a = { }; a[0] = 1;)src",
        {
            eng::assemble(eng::opcode::PUSH_OBJECT),
            0u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::PUSH_VARIABLE),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::SET_OBJECT),
            eng::assemble(eng::opcode::POP),
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 0.0f },
        },
        {
            "a",
        },
        1u);
}

TEST(compiler_tests, access_outer_scope)
{
    check_output(
        R"src(var a = 1; { a = 2; })src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 2.0f },
        },
        {
            "a",
        });
}

TEST(compiler_tests, if_statement)
{
    check_output(
        R"src(if(1 == 1) { var a = 2; })src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::CMP_EQ),
            eng::assemble(eng::opcode::JMP_FALSE),
            4u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 2.0f },
        },
        { });
}

TEST(compiler_tests, while_statement)
{
    check_output(
        R"src(while(true) { var a = 2; })src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::JMP_FALSE),
            6u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::JMP),
            static_cast<std::uint8_t>(-10),
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ true },
            eng::value{ 2.0f },
        },
        { });
}

TEST(compiler_tests, do_while_statement)
{
    check_output(
        R"src(do { var a = 2; } while(false))src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::JMP_FALSE),
            2u,
            eng::assemble(eng::opcode::JMP),
            static_cast<std::uint8_t>(-10),
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 2.0f },
            eng::value{ false },
        },
        { });
}

TEST(compiler_tests, for_statement)
{
    check_output(
        R"src(for(var i = 0; i < 10; i = i + 1) { var b = i; })src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::PUSH_VARIABLE),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::CMP_LESS),
            eng::assemble(eng::opcode::JMP_FALSE),
            13u,
            eng::assemble(eng::opcode::PUSH_VARIABLE),
            0u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            1u,
            eng::assemble(eng::opcode::PUSH_VARIABLE),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            2u,
            eng::assemble(eng::opcode::ADD),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::JMP),
            static_cast<std::uint8_t>(-20),
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 0.0f },
            eng::value{ 10.0f },
            eng::value{ 1.0f },
        },
        { });
}

TEST(compiler_tests, for_each_statement)
{
    check_output(
        R"src(var a = { 0 : 1 }; foreach(var i in a){i;})src",
        {
            eng::assemble(eng::opcode::PUSH_OBJECT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::SET_OBJECT),
            eng::assemble(eng::opcode::POP_VARIABLE),
            0u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            1u,
            eng::assemble(eng::opcode::PUSH_VARIABLE),
            0u,
            eng::assemble(eng::opcode::START_ITER),
            eng::assemble(eng::opcode::PUSH_VARIABLE),
            0u,
            eng::assemble(eng::opcode::ADVANCE_ITER),
            7u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            1u,
            eng::assemble(eng::opcode::PUSH_VARIABLE),
            1u,
            eng::assemble(eng::opcode::POP),
            eng::assemble(eng::opcode::JMP),
            static_cast<std::uint8_t>(-11),
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 0.0f },
        },
        {
            "a"
        },
        1u);
}

TEST(compiler_tests, while_break_statement)
{
    check_output(
        R"src(while(true) { break; })src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::JMP_FALSE),
            4u,
            eng::assemble(eng::opcode::JMP),
            2u,
            eng::assemble(eng::opcode::JMP),
            static_cast<std::uint8_t>(-8),
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ true },
        },
        { });
}

TEST(compiler_tests, simple_function)
{
    const std::vector<std::uint8_t> foo{
        eng::assemble(eng::opcode::PUSH_CONSTANT),
        0u,
        eng::assemble(eng::opcode::RETURN)
    };

    check_output(
        R"src(function foo() { })src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::CLOSURE),
            eng::assemble(eng::opcode::POP_VARIABLE),
            1u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 0.0f },
            eng::value{ eng::function{ &foo, nullptr, 0u } },
        },
        {
            "foo"
        });
}

TEST(compiler_tests, call_function)
{
    const std::vector<std::uint8_t> foo{
        eng::assemble(eng::opcode::PUSH_CONSTANT),
        0u,
        eng::assemble(eng::opcode::RETURN)
    };

    check_output(
        R"src(function foo() { return 5; } var a = foo();)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::CLOSURE),
            eng::assemble(eng::opcode::POP_VARIABLE),
            1u,
            eng::assemble(eng::opcode::PUSH_VARIABLE),
            1u,
            eng::assemble(eng::opcode::CALL),
            0u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            2u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 5.0f },
            eng::value{ eng::function{ &foo, nullptr, 0u } },
        },
        {
            "foo",
            "a",
        });
}

TEST(compiler_tests, call_function_args)
{
    const std::vector<std::uint8_t> foo{
        eng::assemble(eng::opcode::POP_VARIABLE),
        2u,
        eng::assemble(eng::opcode::PUSH_VARIABLE),
        2u,
        eng::assemble(eng::opcode::RETURN)
    };

    check_output(
        R"src(function foo(var x) { return x; } var a = foo(4);)src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::CLOSURE),
            eng::assemble(eng::opcode::POP_VARIABLE),
            1u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            1u,
            eng::assemble(eng::opcode::PUSH_VARIABLE),
            1u,
            eng::assemble(eng::opcode::CALL),
            1u,
            eng::assemble(eng::opcode::POP_VARIABLE),
            3u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ eng::function{ &foo, nullptr, 1u } },
            eng::value{ 4.0f },
        },
        {
            "foo",
            "a",
        });
}

TEST(compiler_tests, function_return_function)
{
    const std::vector<std::uint8_t> foo{
        eng::assemble(eng::opcode::PUSH_CONSTANT),
        1u,
        eng::assemble(eng::opcode::CLOSURE),
        eng::assemble(eng::opcode::POP_VARIABLE),
        2u,
        eng::assemble(eng::opcode::PUSH_VARIABLE),
        2u,
        eng::assemble(eng::opcode::RETURN)
    };

    const std::vector<std::uint8_t> bar{
        eng::assemble(eng::opcode::PUSH_CONSTANT),
        0u,
        eng::assemble(eng::opcode::RETURN)
    };

    check_output(
        R"src(function foo() { function bar() { } return bar; })src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            2u,
            eng::assemble(eng::opcode::CLOSURE),
            eng::assemble(eng::opcode::POP_VARIABLE),
            1u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 0.0f },
            eng::value{ eng::function{ &bar, nullptr, 0u } },
            eng::value{ eng::function{ &foo, nullptr, 0u } },
        },
        {
            "foo"
        });
}

TEST(compiler_tests, closure)
{
    const std::vector<std::uint8_t> foo{
        eng::assemble(eng::opcode::PUSH_CONSTANT),
        0u,
        eng::assemble(eng::opcode::POP_VARIABLE),
        2u,
        eng::assemble(eng::opcode::PUSH_CONSTANT),
        2u,
        eng::assemble(eng::opcode::CLOSURE),
        eng::assemble(eng::opcode::POP_VARIABLE),
        3u,
        eng::assemble(eng::opcode::PUSH_VARIABLE),
        3u,
        eng::assemble(eng::opcode::RETURN)
    };

    const std::vector<std::uint8_t> bar{
        eng::assemble(eng::opcode::PUSH_VARIABLE),
        2u,
        eng::assemble(eng::opcode::PUSH_CONSTANT),
        0u,
        eng::assemble(eng::opcode::ADD),
        eng::assemble(eng::opcode::POP_UPVALUE),
        2u,
        eng::assemble(eng::opcode::PUSH_CONSTANT),
        1u,
        eng::assemble(eng::opcode::RETURN)
    };

    check_output(
        R"src(function foo() { var a = 1; function bar() { a = a + 1; } return bar; })src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            3u,
            eng::assemble(eng::opcode::CLOSURE),
            eng::assemble(eng::opcode::POP_VARIABLE),
            1u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ 1.0f },
            eng::value{ 0.0f },
            eng::value{ eng::function{ &bar, nullptr, 0u } },
            eng::value{ eng::function{ &foo, nullptr, 0u } },
        },
        {
            "foo"
        });
}

TEST(compiler_tests, forward_declared_function)
{
    const std::vector<std::uint8_t> bar{
        eng::assemble(eng::opcode::PUSH_VARIABLE),
        1u,
        eng::assemble(eng::opcode::CALL),
        0u,
        eng::assemble(eng::opcode::RETURN)
    };

    const std::vector<std::uint8_t> foo{
        eng::assemble(eng::opcode::PUSH_CONSTANT),
        1u,
        eng::assemble(eng::opcode::RETURN)
    };

    check_output(
        R"src(function foo() { return bar(); } function bar() { return 5; })src",
        {
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            0u,
            eng::assemble(eng::opcode::CLOSURE),
            eng::assemble(eng::opcode::POP_VARIABLE),
            2u,
            eng::assemble(eng::opcode::PUSH_CONSTANT),
            2u,
            eng::assemble(eng::opcode::CLOSURE),
            eng::assemble(eng::opcode::POP_VARIABLE),
            3u,
            eng::assemble(eng::opcode::STOP)
        },
        {
            eng::value{ eng::function{ &bar, nullptr, 0u } },
            eng::value{ 5.0f },
            eng::value{ eng::function{ &foo, nullptr, 0u } },
        },
        {
            "foo",
            "bar"
        });
}


