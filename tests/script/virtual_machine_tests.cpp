#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <string>

#include "chunk.hpp"
#include "exception.hpp"
#include "location_data.hpp"
#include "opcode.hpp"
#include "virtual_machine.hpp"

TEST(virtual_machine_tests, stop)
{
    eng::chunk c{ };
    c.add_instruction(eng::opcode::STOP);

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_TRUE(vm.globals().empty());
}

TEST(virtual_machine_tests, create_variable)
{
    eng::chunk c{ };
    const auto variable_index = c.add_variable("a");
    const auto const_index = c.add_constant(eng::value{ 1.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ 1.0f } }
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, pop)
{
    eng::chunk c{ };
    const auto const_index = c.add_constant(eng::value{ 1.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index);
    c.add_instruction(eng::opcode::POP);
    c.add_instruction(eng::opcode::STOP);

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_TRUE(vm.globals().empty());
}

TEST(virtual_machine_tests, create_object)
{
    eng::chunk c{ };
    const auto variable_index = c.add_variable("a");
    const auto obj_index = c.add_object();
    c.add_instruction(eng::opcode::PUSH_OBJECT);
    c.add_raw_instruction(obj_index);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index);
    c.add_instruction(eng::opcode::STOP);

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(1u, vm.globals().size());
    ASSERT_TRUE(vm.globals().at("a").is_type<eng::object*>());
}

TEST(virtual_machine_tests, get_object)
{
    eng::chunk c{ };
    eng::object obj_data{ { eng::value{ 2.0f }, eng::value{ 3.0f } } };
    const auto variable_index = c.add_variable("a");
    const auto const_index1 = c.add_constant(eng::value{ &obj_data });
    const auto const_index2 = c.add_constant(eng::value{ 2.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::GET_OBJECT);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index);
    c.add_instruction(eng::opcode::STOP);

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(1u, vm.globals().size());
    ASSERT_EQ(3.0f, vm.globals().at("a").get<float>());
}

TEST(virtual_machine_tests, set_object)
{
    eng::chunk c{ };
    eng::object obj_data{ };
    const auto const_index1 = c.add_constant(eng::value{ &obj_data });
    const auto const_index2 = c.add_constant(eng::value{ 2.0f });
    const auto const_index3 = c.add_constant(eng::value{ 3.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index3);
    c.add_instruction(eng::opcode::SET_OBJECT);
    c.add_instruction(eng::opcode::POP);
    c.add_instruction(eng::opcode::STOP);

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_TRUE(vm.globals().empty());
    ASSERT_EQ(2.0f, obj_data.at(eng::value{ 3.0f }).get<float>());
}

TEST(virtual_machine_tests, add)
{
    eng::chunk c{ };
    const auto variable_index = c.add_variable("a");
    const auto const_index1 = c.add_constant(eng::value{ 2.0f });
    const auto const_index2 = c.add_constant(eng::value{ 3.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::ADD);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ 5.0f } }
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, sub)
{
    eng::chunk c{ };
    const auto variable_index = c.add_variable("a");
    const auto const_index1 = c.add_constant(eng::value{ 2.0f });
    const auto const_index2 = c.add_constant(eng::value{ 3.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::SUB);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ -1.0f } }
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, mul)
{
    eng::chunk c{ };
    const auto variable_index = c.add_variable("a");
    const auto const_index1 = c.add_constant(eng::value{ 2.0f });
    const auto const_index2 = c.add_constant(eng::value{ 3.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::MUL);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ 6.0f } }
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, div)
{
    eng::chunk c{ };
    const auto variable_index = c.add_variable("a");
    const auto const_index1 = c.add_constant(eng::value{ 2.0f });
    const auto const_index2 = c.add_constant(eng::value{ 3.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::DIV);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ 2.0f / 3.0f } }
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, cmp_eq)
{
    eng::chunk c{ };
    const auto variable_index = c.add_variable("a");
    const auto const_index1 = c.add_constant(eng::value{ 2.0f });
    const auto const_index2 = c.add_constant(eng::value{ 2.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::CMP_EQ);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ true } }
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, cmp_neq)
{
    eng::chunk c{ };
    const auto variable_index = c.add_variable("a");
    const auto const_index1 = c.add_constant(eng::value{ 2.0f });
    const auto const_index2 = c.add_constant(eng::value{ 3.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::CMP_NEQ);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ true } }
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, cmp_less)
{
    eng::chunk c{ };
    const auto variable_index1 = c.add_variable("a");
    const auto variable_index2 = c.add_variable("b");
    const auto const_index1 = c.add_constant(eng::value{ 2.0f });
    const auto const_index2 = c.add_constant(eng::value{ 3.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::CMP_LESS);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::CMP_LESS);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index2);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ true } },
        { "b", eng::value{ false } },
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, cmp_less_eq)
{
    eng::chunk c{ };
    const auto variable_index1 = c.add_variable("a");
    const auto variable_index2 = c.add_variable("b");
    const auto const_index1 = c.add_constant(eng::value{ 2.0f });
    const auto const_index2 = c.add_constant(eng::value{ 3.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::CMP_LESS_EQ);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::CMP_LESS_EQ);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index2);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ true } },
        { "b", eng::value{ true } },
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, cmp_more)
{
    eng::chunk c{ };
    const auto variable_index1 = c.add_variable("a");
    const auto variable_index2 = c.add_variable("b");
    const auto const_index1 = c.add_constant(eng::value{ 3.0f });
    const auto const_index2 = c.add_constant(eng::value{ 2.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::CMP_MORE);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::CMP_MORE);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index2);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ true } },
        { "b", eng::value{ false } },
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, cmp_more_eq)
{
    eng::chunk c{ };
    const auto variable_index1 = c.add_variable("a");
    const auto variable_index2 = c.add_variable("b");
    const auto const_index1 = c.add_constant(eng::value{ 2.0f });
    const auto const_index2 = c.add_constant(eng::value{ 3.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::CMP_LESS_EQ);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::CMP_MORE_EQ);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index2);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ true } },
        { "b", eng::value{ true } },
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, jmp)
{
    eng::chunk c{ };
    const auto variable_index1 = c.add_variable("a");
    const auto const_index1 = c.add_constant(eng::value{ 2.0f });
    const auto const_index2 = c.add_constant(eng::value{ 3.0f });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index1);
    c.add_instruction(eng::opcode::JMP);
    c.add_raw_instruction(4u);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(0xff);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ 2.0f } }
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, jmp_false)
{
    eng::chunk c{ };
    const auto variable_index1 = c.add_variable("a");
    const auto const_index1 = c.add_constant(eng::value{ 2.0f });
    const auto const_index2 = c.add_constant(eng::value{ 3.0f });
    const auto const_index3 = c.add_constant(eng::value{ false });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index3);
    c.add_instruction(eng::opcode::JMP_FALSE);
    c.add_raw_instruction(4u);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index2);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(0xff);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ 2.0f } }
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, iterator)
{
    eng::chunk c{ };
    eng::object obj_data{
        { eng::value{ 2.0f }, eng::value{ 3.0f } },
        { eng::value{ 4.0f }, eng::value{ 6.0f } },
    };
    const auto variable_index1 = c.add_variable("a");
    const auto variable_index2 = c.add_variable("b");
    const auto const_index1 = c.add_constant(eng::value{ &obj_data });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::START_ITER);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::ADVANCE_ITER);
    c.add_raw_instruction(0u);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index1);
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index1);
    c.add_instruction(eng::opcode::ADVANCE_ITER);
    c.add_raw_instruction(0u);
    c.add_instruction(eng::opcode::POP_VARIABLE);
    c.add_raw_instruction(variable_index2);
    c.add_instruction(eng::opcode::STOP);

    const std::map<std::string, eng::value> expected {
        { "a", eng::value{ 2.0f } },
        { "b", eng::value{ 4.0f } }
    };

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
    ASSERT_EQ(expected, vm.globals());
}

TEST(virtual_machine_tests, call)
{
    const std::vector<std::uint8_t> func{
        eng::assemble(eng::opcode::STOP)
    };
    const std::vector<eng::location_data> line_data{
        { 0u, 0u }
    };

    eng::chunk c{ };
    const auto const_index = c.add_constant(eng::value{ eng::function{ &func, &line_data, 0u } });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index);
    c.add_instruction(eng::opcode::CLOSURE);
    c.add_instruction(eng::opcode::CALL);
    c.add_raw_instruction(0u);

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
}

TEST(virtual_machine_tests, ret)
{
    const std::vector<std::uint8_t> func{
        eng::assemble(eng::opcode::RETURN)
    };
    const std::vector<eng::location_data> line_data{
        { 0u, 0u }
    };

    eng::chunk c{ };
    const auto const_index = c.add_constant(eng::value{ eng::function{ &func, &line_data, 0u } });
    c.add_instruction(eng::opcode::PUSH_CONSTANT);
    c.add_raw_instruction(const_index);
    c.add_instruction(eng::opcode::CLOSURE);
    c.add_instruction(eng::opcode::CALL);
    c.add_raw_instruction(0u);
    c.add_instruction(eng::opcode::STOP);

    eng::virtual_machine vm{ c };
    ASSERT_NO_THROW(vm.run());
}

