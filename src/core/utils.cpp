#include "core/utils.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace iris
{

bool compare(float a, float b)
{
    static constexpr auto epsilon = std::numeric_limits<float>::epsilon();

    const auto diff = std::fabs(a - b);
    a = std::fabs(a);
    b = std::fabs(b);

    // find largest value
    // use an upper of one to prevent our scaled epsilon getting too large
    const auto largest = std::max({1.0f, a, b});

    // compare using a relative epsilon
    return diff <= (largest * epsilon);
}

}
