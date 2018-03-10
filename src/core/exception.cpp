#include "exception.hpp"

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

#include <cxxabi.h>
#include <execinfo.h>

namespace eng
{

// set upper limit on size of stack trace
static const int max_stack_trace_size = 20u;

// helper alias for creating a unique_ptr which calls std::free on stored
// pointer
template<class T>
using auto_free = std::unique_ptr<T, decltype(&std::free)>;

exception::exception(const std::string &what)
    : std::runtime_error(what),
      frames_(max_stack_trace_size)
{
    // get the stack trace
    const auto frame_count = ::backtrace(frames_.data(), frames_.size());
    frames_.resize(frame_count);

    if(frame_count > 2)
    {
        // remove the first two entries as they are within this class and
        // remove the last as its junk
        frames_ = std::vector<void*>(
            std::cbegin(frames_) + 2u,
            std::cbegin(frames_) + frame_count - 1u);
    }
}

std::string exception::stack_trace() const noexcept
{
    // format all stack frames
    auto_free<char*> symbols{
        ::backtrace_symbols(frames_.data(), frames_.size()),
        &std::free
    };

    std::stringstream strm{ };

    if(symbols)
    {
        auto **symbol_ptr = symbols.get();

        // regex to parse a formatted line and find the C++ mangled name
        std::regex reg{ "[0-9]+\\s*.*\\s0[xX][0-9a-fA-F]+\\s(.*)\\s\\+.*" };

        for(auto i = 0u; i < frames_.size(); ++i)
        {
            std::string symbol{ symbol_ptr[i] };
            std::smatch matches{ };

            // check to see if the regex can find the C++ mangeld name
            if(std::regex_search(symbol,  matches, reg))
            {
                const auto mangled_name = matches[1u].str();

                auto status = -1;

                // attempt to de-mangle it
                auto_free<char> demangled{
                    abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status),
                    &std::free
                };

                if(demangled && (status == 0))
                {
                    // if we did de-mangle the name then replace the mangled
                    // name in the original formatted string
                    const auto split_point = symbol.find(mangled_name);
                    const auto first = symbol.substr(0u, split_point);
                    const auto second = symbol.substr(split_point + mangled_name.size());

                    symbol = first + std::string{ demangled.get() } + second;
                }
            }

            strm << symbol << std::endl;
        }
    }

    return strm.str();
}

}

