#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <string>

#include "chunk.hpp"
#include "exception.hpp"
#include "opcode.hpp"
#include "script_exception.hpp"
#include "virtual_machine.hpp"

namespace
{

void check_errors(
    const eng::chunk &chunk,
    const std::vector<eng::error> &expected)
{
    auto threw = false;
    eng::virtual_machine vm{ chunk };

    try
    {
        vm.run();
    }
    catch(const eng::script_exception &e)
    {
        threw = true;
        EXPECT_EQ(expected, e.errors());
    }

    EXPECT_TRUE(threw);
}

}

TEST(virtual_machine_error_tests, add_incompatible_types)
{
    eng::chunk c{ };
    const auto variable_index = c.add_variable("a");
    const auto const_index1 = c.add_constant(eng::value{ 2.0f });
    const auto const_index2 = c.add_constant(eng::value{ true });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::ADD);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index);
    c.add_instruction(eng::opcode::STOP);

    check_errors(c, { eng::error( eng::location_data{ 0u, 0u }, eng::error_type::RUNTIME_ERROR) });
}

TEST(virtual_machine_error_tests, push_unknown_variable)
{
    eng::chunk c{ };
    c.add_instruction(eng::opcode::PUSH_VARIABLE);
    c.add_raw_instruction(0xff);

    check_errors(c, { eng::error( eng::location_data{ 0u, 0u }, eng::error_type::RUNTIME_ERROR) });
}

TEST(virtual_machine_error_tests, push_unknown_upvalue)
{
    eng::chunk c{ };
    c.add_instruction(eng::opcode::POP_UPVALUE);
    c.add_raw_instruction(0xff);

    check_errors(c, { eng::error( eng::location_data{ 0u, 0u }, eng::error_type::RUNTIME_ERROR) });
}

TEST(virtual_machine_error_tests, advance_iter_imbalance)
{
    eng::chunk c{ };
    const auto const_index = c.add_constant(eng::value{ 2.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index);
    c.add_instruction(eng::opcode::ADVANCE_ITER);
    c.add_raw_instruction(0xff);

    check_errors(c, { eng::error( eng::location_data{ 0u, 0u }, eng::error_type::ITERATOR_IMBALANCE) });
}

TEST(virtual_machine_error_tests, stack_imbalance)
{
    eng::chunk c{ };
    const auto const_index = c.add_constant(eng::value{ 2.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index);
    c.add_instruction(eng::opcode::STOP);

    check_errors(c, { eng::error( eng::location_data{ 0u, 0u }, eng::error_type::STACK_IMBALANCE) });
}

TEST(virtual_machine_error_tests, unknown_opcode)
{
    eng::chunk c{ };
    c.add_raw_instruction(0xff);

    check_errors(c, { eng::error( eng::location_data{ 0u, 0u }, eng::error_type::UNKNOWN_OPCODE) });
}

TEST(virtual_machine_error_tests, call_incorrect_args)
{
    const std::vector<std::uint8_t> func{
        eng::assemble(eng::opcode::RETURN)
    };
    eng::chunk c{ };
    const auto const_index = c.add_constant(eng::value{ eng::function{ &func, nullptr, 0u } });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index);
    c.add_instruction(eng::opcode::CLOSURE);
    c.add_instruction(eng::opcode::CALL);
    c.add_raw_instruction(1u);
    c.add_instruction(eng::opcode::STOP);

    check_errors(c, { eng::error( eng::location_data{ 0u, 0u }, eng::error_type::INCORRECT_ARG_NUM) });
}

