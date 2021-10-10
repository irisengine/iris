////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>

namespace iris
{

/**
 * This is an interface for 3D shape used for collision detection and
 * resolution.
 */
class CollisionShape
{
  public:
    virtual ~CollisionShape() = default;
};

}
