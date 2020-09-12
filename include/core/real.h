#pragma once

#include <cmath>
#include <iostream>
#include <type_traits>

namespace iris
{

/**
 * Lightweight wrapper around a float. This provides (as much as possible) the
 * same interface as a native float except for equality, which uses a relative
 * epsilon comparison to account for limitations in floats.
 */
class real
{
    public:
         
        using type = float;

        /**
         * Construct a real from an arithmetic type. Deliberately non-explicit
         * for a float-like API.
         *
         * @param value
         *   Value of real.
         */
        template<class T, typename=std::enable_if_t<std::is_arithmetic_v<T>>>
        real(T value)
            : value(value)
        {
            // check there are no padding shenanigans
            static_assert(sizeof(*this) == sizeof(type));
        }

        /**
         * Cast to float.
         */
        operator type&()
        {
            return value;
        }

        /**
         * Cast to float.
         */
        operator type() const
        {
            return value;
        }

        /** Value of real. */
        type value;
};

// comparison operators
// provided for comparing real vs real and real vs floating-point type

inline bool operator==(real value1, real value2)
{
    static constexpr auto epsilon = std::numeric_limits<real::type>::epsilon();

    const auto diff = std::fabs(value1 - value2);
    value1 = std::fabs(value1);
    value2 = std::fabs(value2);

    // find largest value
    // use an upper of one to prevent our scaled epsilon getting too large
    const auto largest = std::max({ 1.0f, value1.value, value2.value });

    // compare using a relative epsilon
    return diff <= (largest * epsilon);
}

template<class T, typename=std::enable_if_t<std::is_floating_point_v<T>>>
inline bool operator==(real value1, T value2)
{
    return value1 == real{ value2 };
}

inline bool operator!=(real value1, real value2)
{
    return !(value1.value == value2.value);
}

template<class T, typename=std::enable_if_t<std::is_floating_point_v<T>>>
inline bool operator!=(real value1, T value2)
{
    return !(value1.value == value2);
}

inline bool operator<(real value1, real value2)
{
    return value1.value < value2.value;
}

template<class T, typename=std::enable_if_t<std::is_floating_point_v<T>>>
inline bool operator<(real value1, T value2)
{
    return value1.value < value2;
}

inline bool operator<=(real value1, real value2)
{
    return (value1.value < value2.value) || (value1.value == value2.value);
}

template<class T, typename=std::enable_if_t<std::is_floating_point_v<T>>>
inline bool operator<=(real value1, T value2)
{
    return (value1.value < value2) || (value1.value == value2);
}

inline bool operator>=(real value1, real value2)
{
    return !(value1 < value2);
}

template<class T, typename=std::enable_if_t<std::is_floating_point_v<T>>>
inline bool operator>=(real value1, T value2)
{
    return !(value1 < value2);
}

inline bool operator>(real value1, real value2)
{
    return !(value1 <= value2);
}

template<class T, typename=std::enable_if_t<std::is_floating_point_v<T>>>
inline bool operator>(real value1, T value2)
{
    return !(value1 <= value2);
}

inline std::ostream& operator<<(std::ostream &out, real r)
{
    out << r.value;
    return out;
}

inline std::istream& operator>>(std::istream &in, real &r)
{
    in >> r.value;
    return in;
}

}

