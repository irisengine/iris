////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace iris
{

/**
 * Class which records stack traces as they are generated and pretty prints the final profile stats.
 */
class ProfilerAnalyser
{
  public:
    /**
     * Add a stack trace to the analyser.
     *
     * @param stack_trace
     *   The stack trace to add.
     */
    void add_stack_trace(const std::vector<std::string> &stack_trace);

    /**
     * Pretty print all generated profile stats to stdout.
     */
    void print();

  private:
    /**
     * Struct to encapsulate recorded data.
     */
    struct Level
    {
        std::uint32_t hit_count = 0u;
        std::string name;
        std::vector<Level> children;
    };

    /** Start of the profiling heirarchy. */
    Level root_level_;
};

}
