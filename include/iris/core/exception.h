#pragma once

#include <stdexcept>
#include <string>

namespace iris
{

/**
 * The general exception type for the engine.
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
};

}
