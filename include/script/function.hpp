#pragma once

#include <cstdint>
#include <cstddef>
#include <map>
#include <vector>

#include "cactus_stack.hpp"
#include "location_data.hpp"

namespace eng
{

// forward declaration
class value;

/**
 * Struct encapsulating a callable function value. This can be stored in a
 * value object and be put on the stack in the virtual machine.
 */
struct function
{
    /**
     * Construct a new function.
     *
     * @param code
     *   Pointer to the functions opcodes.
     *
     * @param line_data
     *   Pointer to the location data for the function.
     *
     * @param num_args
     *   The number of arguments the function requires.
     */
    function(
        const std::vector<std::uint8_t> *code,
        const std::vector<location_data> *line_data,
        const std::size_t num_args)
        : code(code),
          line_data(line_data),
          num_args(num_args),
          variables()
    { }

    // default
    ~function() = default;
    function(const function&) = default;
    function& operator=(const function&) = default;
    function(function&&) = default;
    function& operator=(function&&) = default;

    /**
     * Equality operator. Check whether the pointed to code and line data
     * are equal, not that they point to the same objects.
     *
     * @param other
     *   function to compare with.
     *
     * @returns
     *   True if equal, false otherwise.
     */
    bool operator==(const function &other) const
    {
        return (*code == *other.code) && (num_args == other.num_args);
    }

    /**
     * Inequality operator. Check whether the pointed to code and line data
     * are not equal, not that they point to the same objects.
     *
     * @param other
     *   function to compare with.
     *
     * @returns
     *   True if not equal, false otherwise.
     */
    bool operator!=(const function &other) const
    {
        return !(*this == other);
    }

    /** Pointer to collection of opcodes. */
    const std::vector<std::uint8_t> *code;

    /** Pointer to line data that corresponds to opcodes */
    const std::vector<location_data> *line_data;

    /** Number of arguments for function. */
    std::size_t num_args;

    /**
     * Iterator to the top of the variables cactus_stack at the point this
     * function was created. This allows calls to access variables that where
     * in-scope at the point a closure was created.
     */
    cactus_stack<std::map<std::uint32_t, value>>::iterator variables;
};

}

