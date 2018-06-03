#include "chunk.hpp"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

#include "exception.hpp"
#include "location_data.hpp"
#include "opcode.hpp"
#include "token.hpp"
#include "value.hpp"

namespace eng
{

chunk::chunk()
    : constants_(),
      num_objects_(0u),
      variable_scope_(),
      variable_counter_(0u),
      function_scope_(),
      location_data_()
{
    // initialise chunk with a scope for global variables and a 'main' function
    push_scope();
    push_function();
}

std::size_t chunk::add_instruction(const opcode instruction)
{
    return add_instruction(instruction, { 0u, 0u });
}

std::size_t chunk::add_instruction(
    const opcode instruction,
    const token tok)
{
    return add_instruction(instruction, tok.location);
}

std::size_t chunk::add_instruction(
    const opcode instruction,
    const location_data &location)
{
    return add_raw_instruction(assemble(instruction), location);
}

std::size_t chunk::last_instruction() const
{
    return function_scope_.top().size() - 1u;
}

std::size_t chunk::add_constant(value constant)
{
    // see if value has already been added
    const auto iter = std::find_if(
        std::cbegin(constants_),
        std::cend(constants_),
        [&constant] (const value &val)
        {
            return val.is_same(constant) && (val == constant);
        });

    // get index of constant
    const auto index = std::distance(std::cbegin(constants_), iter);

    // if constant is new then add it, this does not invalidate our computed
    // index
    if(iter == std::cend(constants_))
    {
        constants_.emplace_back(std::move(constant));
    }

    return index;
}

std::size_t chunk::add_object()
{
    ++num_objects_;
    return num_objects_ - 1u;
}

std::size_t chunk::add_variable(const std::string name)
{
    // each variable has a unique id, regardless of its scope
    variable_scope_.top().insert_or_assign(std::move(name), variable_counter_);
    ++variable_counter_;

    return variable_counter_ - 1u;
}

void chunk::patch_jump(const std::size_t jump, const std::size_t landing)
{
    if(jump >= function_scope_.top().size())
    {
        throw exception("jump point does not exist");
    }

    if(landing >= function_scope_.top().size())
    {
        throw exception("landing point does not exist");
    }

    const auto arg =
        static_cast<std::underlying_type<opcode>::type>(landing - jump);
    function_scope_.top().at(jump) = arg;
}

void chunk::push_scope()
{
    variable_scope_.push({ });
}

void chunk::pop_scope()
{
    variable_scope_.pop();
}

const cactus_stack<std::map<std::string, std::uint32_t>>& chunk::variables() const
{
    return variable_scope_;
}

void chunk::push_function()
{
    function_scope_.push({ });
    location_data_.push({ });
}

void chunk::pop_function()
{
    function_scope_.pop();
    location_data_.pop();
}

const cactus_stack<std::vector<std::uint8_t>>& chunk::functions() const
{
    return function_scope_;
}

const cactus_stack<std::vector<location_data>>& chunk::line_data() const
{
    return location_data_;
}

std::vector<std::uint8_t> chunk::code() const
{
    return function_scope_.top();
}

const std::vector<value>& chunk::constants() const
{
    return constants_;
}

std::uint32_t chunk::num_objects() const
{
    return num_objects_;
}

std::ostream& operator<<(std::ostream &strm, const chunk &c)
{
    strm << std::hex;
    strm << disassemble(c.code()) << std::endl;
    strm << std::dec;

    strm << "== constants ==" << std::endl << std::endl;

    int counter = 0;

    for(const auto &con : c.constants_)
    {
        strm << counter << ": ";
        ++counter;

        strm << con << std::endl;
    }

    strm << "== functions ==" << std::endl << std::endl;

    for(const auto &con : c.constants_)
    {
        if(con.is_type<eng::function>())
        {
            std::cout << disassemble(*con.get<eng::function>().code) << std::endl;
        }
    }

    return strm;
}

}

