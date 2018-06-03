#pragma once

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <map>
#include <vector>

#include "cactus_stack.hpp"
#include "location_data.hpp"
#include "opcode.hpp"
#include "token.hpp"
#include "value.hpp"

namespace eng
{

/**
 * This class encapsulates all the data for compiled code to be run on a
 * virtual machine. It will be produced by the compiler. As well as code it
 * also contains data about variables and functions.
 *
 * Internally this class uses cactus_stack containers to keep track of variables
 * and functions. Methods are provided to push and pop new 'scopes' for each
 * of these. When added code or variables they are always added to the current
 * scope.
 *
 * When constructed the initial function scope is the 'main' and the initial
 * variable scope are globals.
 */
class chunk final
{
    public:

        /**
         * Construct a new chunk class.
         */
        chunk();

        // default
        ~chunk() = default;
        chunk(const chunk&) = default;
        chunk& operator=(const chunk&) = default;
        chunk(chunk&&) = default;
        chunk& operator=(chunk&&) = default;

        /**
         * Add an instruction to the current function scope. Will have a default
         * location at the start of the script.
         *
         * @param instruction
         *   Opcode to add.
         *
         * @returns
         *   The index of the inserted instruction in the current function scope.
         */
        std::size_t add_instruction(const opcode instruction);

        /**
         * Add an instruction to the current function scope.
         *
         * @param instruction
         *   Opcode to add.
         *
         * @param tok
         *   The token associated with the instruction, this is used for error
         *   location information.
         *
         * @returns
         *   The index of the inserted instruction in the current function scope.
         */
        std::size_t add_instruction(
            const opcode instruction,
            const token tok);

        /**
         * Add an instruction to the current function scope.
         *
         * @param instruction
         *   Opcode to add.
         *
         * @param location
         *   Location in script file associated with instruction.
         *
         * @returns
         *   The index of the inserted instruction in the current function scope.
         */
        std::size_t add_instruction(
            const opcode instruction,
            const location_data &location);

        /**
         * Add a raw instruction to the current function scope. Will have a
         * default location at the start of the script. This is used when a raw
         * (i.e. non opcode) value needs to be added, such as an opcode
         * argument.
         *
         * Note that this is templated to accept any integral type. This *will*
         * be narrowed to the underlying integer type of the opcode enum.
         *
         * @param instruction
         *   Opcode to add.
         *
         * @returns
         *   The index of the inserted instruction in the current function scope.
         */
        template<
            class T,
            typename=std::enable_if_t<std::is_integral<T>::value>>
        std::size_t add_raw_instruction(const T instruction)
        {
            return add_raw_instruction(instruction, { 0u, 0u });
        }

        /**
         * Add a raw instruction to the current function scope. This is used
         * when a raw (i.e. non opcode) value needs to be added, such as an
         * opcode argument.
         *
         * Note that this is templated to accept any integral type. This *will*
         * be narrowed to the underlying integer type of the opcode enum.
         *
         * @param instruction
         *   Opcode to add.
         *
         * @param tok
         *   The token associated with the instruction, this is used for error
         *   location information.
         *
         * @returns
         *   The index of the inserted instruction in the current function scope.
         */
        template<
            class T,
            typename=std::enable_if_t<std::is_integral<T>::value>>
        std::size_t add_raw_instruction(const T instruction, const token tok)
        {
            return add_raw_instruction(instruction, tok.location);
        }

        /**
         * Add a raw instruction to the current function scope. This is used
         * when a raw (i.e. non opcode) value needs to be added, such as an
         * opcode argument.
         *
         * Note that this is templated to accept any integral type. This *will*
         * be narrowed to the underlying integer type of the opcode enum.
         *
         * @param instruction
         *   Opcode to add.
         *
         * @param location
         *   Location in script file associated with instruction.
         *
         * @returns
         *   The index of the inserted instruction in the current function scope.
         */
        template<
            class T,
            typename=std::enable_if_t<std::is_integral<T>::value>>
        std::size_t add_raw_instruction(
            const T instruction,
            const location_data location)
        {
            const auto raw_instruction =
                static_cast<std::underlying_type<opcode>::type>(instruction);

            function_scope_.top().emplace_back(raw_instruction);
            location_data_.top().emplace_back(location);

            return last_instruction();
        }

        /**
         * Get the index into the current function scope of the last added
         * instruction.
         *
         * @returns
         *   Index of last added instruction.
         */
        std::size_t last_instruction() const;

        /**
         * Add a constant value. If the constant has already been added then
         * the original index will be returned and no new addition will be made.
         *
         * @constant
         *   Constant to add
         *
         * @returns
         *   Index of added constant, or original index if constant has already
         *   been added.
         */
        std::size_t add_constant(value constant);

        /**
         * Add a new object.
         *
         * @returns
         *   Index of new object.
         */
        std::size_t add_object();

        /**
         * Add a variable to the current variable scope.
         *
         * @param name
         *   Name of variable.
         *
         * @returns
         *   Index of new variable in current variable scope.
         */
        std::size_t add_variable(const std::string name);

        /**
         * Push a new variable scope.
         */
        void push_scope();

        /**
         * Pop the current variable scope.
         */
        void pop_scope();

        /**
         * Get a reference to the stored variables.
         *
         * @returns
         *   A reference to the cactus_stack of variables.
         */
        const cactus_stack<std::map<std::string, std::uint32_t>>& variables() const;

        /**
         * Push a new function scope.
         */
        void push_function();

        /**
         * Pop the current function scope.
         */
        void pop_function();

        /**
         * Get a reference to the stored functions.
         *
         * @returns
         *   A reference to the cactus_stack of functions.
         */
        const cactus_stack<std::vector<std::uint8_t>>& functions() const;

        /**
         * Get a reference to the stored line data, this should match the
         * stored functions.
         *
         *
         * @returns
         *   A reference to the cactus_stack of line data.
         */
        const cactus_stack<std::vector<location_data>>& line_data() const;

        /**
         * Get the code of the main function.
         *
         * @returns
         *   Assembly of main.
         */
        std::vector<std::uint8_t> code() const;

        /**
         * Get a reference to the stored constants.
         *
         * @returns
         *   Stored constants.
         */
        const std::vector<value>& constants() const;

        /**
         * Get the number of objects in the current program.
         *
         * @returns
         *   Number of objects.
         */
        std::uint32_t num_objects() const;

        /**
         * Patch a jump instruction in the current scope. This used when a
         * jump instruction has been assembled but needs to jump forward, so
         * a placeholder jump distance will have been used.
         *
         * @param jump
         *   Index of jump instruction.
         *
         * @param landing
         *   Index of instruction to jump to.
         */
        void patch_jump(const std::size_t jump, const std::size_t landing);

        /**
         * Write a chunk to a stream, useful for debugging.
         *
         * @param out
         *   Stream to write to.
         *
         * @param c
         *   chunk to write to stream.
         *
         * @return
         *   Reference to input stream.
         */
        friend std::ostream& operator<<(std::ostream &strm, const chunk &c);

    private:

        /** Collection of constants. */
        std::vector<value> constants_;

        /** Number of objects in chunk. */
        std::uint32_t num_objects_;

        /** cactus_stack of variables. */
        cactus_stack<std::map<std::string, std::uint32_t>> variable_scope_;

        /** Total number of added variables. */
        std::uint32_t variable_counter_;

        /** cactus_stack of functions. */
        cactus_stack<std::vector<std::uint8_t>> function_scope_;

        /** cactus_stack of location data. */
        cactus_stack<std::vector<location_data>> location_data_;
};

}

