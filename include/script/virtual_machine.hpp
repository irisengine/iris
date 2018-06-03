#pragma once

#include <cstdint>
#include <map>
#include <stack>
#include <tuple>
#include <vector>

#include "cactus_stack.hpp"
#include "chunk.hpp"
#include "location_data.hpp"
#include "object.hpp"
#include "value.hpp"

namespace eng
{

/**
 * Class responsible for running compiled scripts. Each execution of the script
 * is idempotent and will reset all internal data before running.
 */
class virtual_machine final
{
    public:

        /** Construct a new virtual_machine.
         *
         * @param chnk
         *   The chunk to run.
         */
        virtual_machine(const chunk &chnk);

        // default
        ~virtual_machine() = default;
        virtual_machine(const virtual_machine&) = default;
        virtual_machine& operator=(const virtual_machine&) = default;
        virtual_machine(virtual_machine&&) = default;
        virtual_machine& operator=(virtual_machine&&) = default;

        /**
         * Execute the script. Each call to this is idempotent and will reset
         * internal data before running.
         */
        void run();

        /**
         * Get the global variables from the script execution. Calling this
         * method before running a script will produce an empty collection.
         *
         * @returns
         *   Script global variables.
         */
        std::map<std::string, value> globals() const;

    private:

        /** Chunk to execute. */
        chunk chunk_;

        /** Program counter, pointer to currently executing instruction. */
        const std::uint8_t *pc_;

        /** Pointer to line data for current instruction. */
        const location_data *line_data_;

        /** Execution stack. */
        std::stack<value> stack_;

        /** Variables created during execution. */
        cactus_stack<std::map<std::uint32_t, value>> variables_;

        /** Collection of iterators created during execution. */
        std::stack<object::iterator> iterators_;

        /** Collection of object created during execution. */
        std::vector<std::unique_ptr<object>> objects_;

        /**
         * Call stack, used for calling and returning from functions.
         * When a function is called then the following data is stored in this
         * stack;
         *   - current pc
         *   - current location data
         *   - iterator to current location in variable cactus_stack
         * This allows a return to restore the internal state of the virtual
         * machine so we can 'jump' back to the call point
         */
        std::stack<std::tuple<const std::uint8_t*, const location_data*, cactus_stack<std::map<std::uint32_t, value>>::iterator>> call_stack_;
};

}

