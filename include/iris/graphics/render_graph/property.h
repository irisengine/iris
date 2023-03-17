////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>
#include <cstddef>
#include <string>

namespace iris
{

/**
 * Enumeration of possible property types.
 */
enum class PropertyType
{
    FLOAT
};

/**
 * This class represents the cpu side of a property.
 *
 * It has a very restricted API (see PropertyWriter for public usage) but internally it has a pointer to a buffer it
 * can write property values to, the engine will then ensure it gets copied to the cpu.
 */
class Property
{
  public:
    ~Property();

    /**
     * Get property name.
     *
     * @returns
     *   User name of property.
     */
    std::string name() const;

    /**
     * Get type of property.
     *
     * @returns
     *   Property type.
     */
    PropertyType type() const;

  private:
    // we use friend classes to lock down the API and hand out access to various other classes
    template <class>
    friend class PropertyWriter;           // for writing
    friend class std::allocator<Property>; // for storing
    friend class RenderGraph;              // for creation

    /**
     * Construct a new Property storing a float.
     *
     * @param name
     *    User name of property.
     *
     * @param buffer
     *   Pointer to memory to write value to.
     *
     * @param value
     *   Initial value of property.
     */
    Property(const std::string &name, std::byte *buffer, float value);

    /**
     * Set the float value of the property.
     *
     * It is undefined to call this of the object was created with the float constructor, although the restricted API
     * should make that hard to do.
     *
     * @param value
     *   New value.
     */
    void set_value(float value);

    /** User name of parameter. */
    std::string name_;

    /** Parameter type. */
    PropertyType type_;

    /** Pointer to write values to. */
    std::byte *buffer_;
};

}
