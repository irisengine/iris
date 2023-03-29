////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "vector3.h"

namespace iris
{

class Vector4
{
  public:
    constexpr Vector4()
        : Vector4(0.0f)
    {
    }

    constexpr Vector4(float xyzw)
        : Vector4(xyzw, xyzw, xyzw, xyzw)
    {
    }

    constexpr Vector4(float x, float y, float z, float w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    constexpr Vector4(const Vector3 &v, float w = 0.0f)
        : Vector4(v.x, v.y, v.z, w)
    {
    }

    constexpr Vector4 &operator/=(float scale)
    {
        x /= scale;
        y /= scale;
        z /= scale;
        w /= scale;

        return *this;
    }

    constexpr Vector4 operator/(float scale) const
    {
        return Vector4(*this) /= scale;
    }

    constexpr Vector3 xyz() const
    {
        return {x, y, z};
    }

    float x;
    float y;
    float z;
    float w;
};

}