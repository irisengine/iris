#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace iris
{

/**
 * A custom exception class that includes a stack trace.
 */
class Exception : public std::runtime_error
{
  public:
    /**
     * Construct a new exception.
     *
     * @param what
     *   Exception message.
     */
    Exception(const std::string &what);

    /**
     * Get a formatted copy of the stack trace. Performs C++ name
     * de-mangling where possible.
     *
     * @returns
     *   Formatted stack trace, or empty string if no stack trace was
     *   available or it could not be formatted.
     */
    std::string stack_trace() const;

  private:
    /** Collection of pointers to stack frames to stack trace. */
    std::vector<void *> frames_;
};

}
