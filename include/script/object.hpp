#pragma once

#include <map>

namespace eng
{

// forward declaration
class value;

/**
 * An object is a script variable type which maps any value to any value.
 */
using object = std::map<value, value>;

}

