////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

namespace utils::detail
{

/**
 * Helper function to combine the hash values of supplied objects (using std::hash).
 *
 * @param seed
 *   Initial hash value, will be set to combined hash value.
 *
 * @param head
 *   Head of recursion.
 *
 * @param tail
 *   Remainder of recursion.
 */
template <class Head, class... Tail>
void do_combine_hash(std::size_t &seed, const Head &head, Tail... tail)
{
    // this algorithm is based on boost::combine_hash and is designed to mix/smear around values
    seed ^= std::hash<Head>{}(head) + 0x9e3779b97f4a7c55 + (seed << 6) + (seed >> 2);
    (do_combine_hash(seed, tail), ...);
}

}

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

/**
 * Combine the hash values of supplied objects (using std::hash).
 *
 * @param args
 *   Variable list of objects to hash.
 *
 * @return
 *   Combined hash value of all objects.
 */
template <class... Args>
std::size_t combine_hash(Args... args)
{
    std::size_t seed = 0u;
    utils::detail::do_combine_hash(seed, args...);
    return seed;
}

}
