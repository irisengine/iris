#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace eng
{

/**
 * A custom exception class that includes a stack trace.
 */
class exception : public std::runtime_error
{
    public:

        /**
         * Construct a new exception.
         *
         * @param what
         *   Exception message.
         */
        exception(const std::string &what);

        /** Default*/
        ~exception() override = default;
        exception(const exception&) = default;
        exception& operator=(const exception&) = default;
        exception(exception&&) = default;
        exception& operator=(exception&&) = default;

        /**
         * Get a formatted copy of the stack trace. Performs C++ name
         * de-mangling where possible.
         *
         * @returns
         *   Formatted stack trace, or empty string if no stack trace was
         *   available or it could not be formatted.
         */
        std::string stack_trace() const noexcept;

    private:

        /** Collection of pointers to stack frames to stack trace. */
        std::vector<void*> frames_;
};

}

