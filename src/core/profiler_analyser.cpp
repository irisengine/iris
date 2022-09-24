////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/profiler_analyser.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <stack>
#include <vector>

namespace iris
{

void ProfilerAnalyser::add_stack_trace(const std::vector<std::string> &stack_trace)
{
    Level *cursor = &root_level_;

    // walk back through the stack trace
    for (auto iter = std::crbegin(stack_trace); iter != std::crend(stack_trace); ++iter)
    {
        const auto &function = *iter;

        // see if the current function has already been seen at this level
        auto child =
            std::find_if(std::begin(cursor->children), std::end(cursor->children), [&function](const auto &level) {
                return level.name == function;
            });

        if (child == std::end(cursor->children))
        {
            // if it hasn't then add it
            cursor->children.push_back({.name = function});
            ++cursor->hit_count;
            cursor = std::addressof(cursor->children.back());
        }
        else
        {
            // else then keep searching from the child
            ++cursor->hit_count;
            cursor = std::addressof(*child);
        }
    }
}

void ProfilerAnalyser::print()
{
    const auto total_hits = root_level_.hit_count;
    std::stack<std::tuple<Level *, std::uint32_t>> stack;
    stack.emplace(&root_level_, 0u);

    // depth first walk all the recorded levels
    while (!stack.empty())
    {
        auto [level, indent] = stack.top();
        stack.pop();

        // sort the children so we print most hit first
        std::sort(std::begin(level->children), std::end(level->children), [](const auto &a, const auto &b) {
            return a.hit_count < b.hit_count;
        });

        const auto percentage = static_cast<float>(level->hit_count) / static_cast<float>(total_hits);

        // print out line
        std::cout << "|-" << std::string(indent, '-') << level->name << " (" << level->hit_count << " | "
                  << percentage * 100.0f << ")" << std::endl;

        for (auto &child : level->children)
        {
            stack.emplace(std::addressof(child), indent + 1u);
        }
    }
}

}
