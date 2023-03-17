////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/property.h"

#include <array>
#include <cstddef>
#include <memory>
#include <string>

#include "core/error_handling.h"

namespace
{

/**
 * Helper function to write value into buffer.
 *
 * @param buffer
 *   Where to write value to.
 *
 * @param value
 *   Value to write.
 */
template <class T>
void do_set_value(std::byte *buffer, const T &value)
{
    // by convention the buffer always has a value - so destruct it first
    std::destroy_at(reinterpret_cast<T *>(buffer));

    // write the new value in via its copy constructor
    std::construct_at(reinterpret_cast<T *>(buffer), value);
}

}

namespace iris
{

Property::Property(const std::string &name, std::byte *buffer, float value)
    : name_(name)
    , type_(PropertyType::FLOAT)
    , buffer_(buffer)
{
    expect(buffer_ != nullptr, "must supply buffer for value");
    std::construct_at(reinterpret_cast<float *>(buffer_), value);
}

Property::~Property()
{
    // ensure correct destructor is called
    switch (type_)
    {
        using enum PropertyType;

        case FLOAT: std::destroy_at(reinterpret_cast<float *>(buffer_)); break;
        default: expect(false, "unknown property type");
    }
}

std::string Property::name() const
{
    return name_;
}

PropertyType Property::type() const
{
    return type_;
}

void Property::set_value(float value)
{
    do_set_value(buffer_, value);
}

}