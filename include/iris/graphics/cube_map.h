////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace iris
{

/**
 * Interface for a cube map i.e. a texture with six slices, one for each face of a cube.
 */
class CubeMap
{
  public:
    virtual ~CubeMap() = default;
};

}
