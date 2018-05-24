#include "error.hpp"

#include "error_type.hpp"
#include "location_data.hpp"

namespace eng
{

error::error(
    const location_data location,
    const error_type type)
    : location_(location),
      type_(type)
{ }

location_data error::location() const
{
    return location_;
}

error_type error::type() const
{
    return type_;
}

bool error::operator==(const error &other) const
{
    return
        (location_ == other.location_) &&
        (type_ == other.type_);
}

bool error::operator!=(const error &other) const
{
    return !(*this == other);
}

}

