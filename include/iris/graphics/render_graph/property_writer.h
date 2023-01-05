////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/error_handling.h"
#include "graphics/render_graph/property.h"

namespace iris
{

/**
 * This class is used to write to Property objects. It's templated to restrict what types can be written, so for example
 * only floats can be written to a float property.
 */
template <class T>
class PropertyWriter
{
  public:
    /**
     * Construct a new PropertyWriter.
     */
    PropertyWriter()
        : property_(nullptr)
    {
    }

    /**
     * Set the value of the property.
     *
     * @param value
     *   New value.
     */
    void set_value(const T &value)
    {
        expect(property_ != nullptr, "no property set");
        property_->set_value(value);
    }

  private:
    friend class RenderGraph;

    /**
     * Construct a new PropertyWriter with a Property.
     *
     * @param property
     *   Property to write to.
     */
    PropertyWriter(Property *property)
        : property_(property)
    {
    }

    /** Property to write to. */
    Property *property_;
};

}
