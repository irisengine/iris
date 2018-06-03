#include "script_exception.hpp"

namespace eng
{

script_exception::script_exception(const error &err)
    : script_exception(std::vector<error>{ err })
{ }

script_exception::script_exception(const std::vector<error> &errors)
        : exception("lexer error"),
          errors_(errors)
{ }

std::vector<error> script_exception::errors() const
{
    return errors_;
}

}

