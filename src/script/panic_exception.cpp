#include "panic_exception.hpp"

#include "error.hpp"
#include "token.hpp"

namespace eng
{

panic_exception::panic_exception(const error_type type, const token &bad_token)
    : exception("panic in compiling script"),
      error_type_(type),
      bad_token_(bad_token)
{ }

error_type panic_exception::type() const
{
    return error_type_;
}

token panic_exception::bad_token() const
{
    return bad_token_;
}

}

