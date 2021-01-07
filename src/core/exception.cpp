#include "core/exception.h"

#include <stdexcept>
#include <string>

namespace iris
{

Exception::Exception(const std::string &what)
    : std::runtime_error(what)
{
}

}
