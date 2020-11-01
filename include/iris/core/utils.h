#pragma once

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
