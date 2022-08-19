////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

namespace iris
{

/**
 * Interface for a Material - a class which which encapsulates how to render a Mesh.
 *
 * This interface is deliberately limited, most of the functionality is provided by the implementations, which in turn
 * is only used internally by the engine.
 */
class Material
{
  public:
    virtual ~Material() = default;
};

}
