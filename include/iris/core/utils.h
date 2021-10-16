////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

namespace iris
{

/**
 * Compare two floating point numbers using a scaling epsilon.
 *
 * @param a
 *   First float.
 *
 * @param b
 *   Second float.
 *
 * @returns
 *   True if both floats are equal (within an epsilon), false otherwise.
 */
bool compare(float a, float b);

}
