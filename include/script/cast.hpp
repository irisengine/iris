#pragma once

#include <memory>
#include <type_traits>

#include "token.hpp"
#include "token_type.hpp"

namespace eng
{

/**
 * Helper function to test if a base pointer is actually a derived class.
 *
 * This uses custom RTTI and required that the derived object has a static
 * bool is(S*) method which can tell whether requested object is of the
 * correct type.
 *
 * @param obj
 *   Base unique_ptr to test type.
 *
 * @returns
 *   True if supplied pointer is the requested derived type, false otherwise.
 */
template<
    class Derived,
    class Base,
    typename = std::enable_if_t<std::is_base_of<Base, Derived>::value>>
inline bool is(const std::unique_ptr<Base> &obj)
{
    return Derived::is(obj.get());
}

/**
 * Helper function to test if a base pointer is actually a derived class.
 *
 * This uses custom RTTI and required that the derived object has a static
 * bool is(S*) method which can tell whether requested object is of the
 * correct type.
 *
 * @param obj
 *   Base raw pointer to test type.
 *
 * @returns
 *   True if supplied pointer is the requested derived type, false otherwise.
 */
template<
    class Derived,
    class Base,
    typename = std::enable_if_t<std::is_base_of<Base, Derived>::value>>
inline bool is(const Base *obj)
{
    return Derived::is(obj);
}

/**
 * Helper function to test token type.
 *
 * This is mainly here to have a consistent is<T> style API between tokens,
 * expressions and statements.
 *
 * @param token
 *   Token to test type of.
 *
 * @returns
 *   True if any if the token is any of the supplied token types.
 */
template<eng::token_type ...Types>
inline bool is(const eng::token &token)
{
    // construct a collection from all the supplied types
    const auto types = { Types... };

    // see if our type is in the supplied collection
    const auto find = std::find(std::cbegin(types), std::cend(types), token.type);
    return find != std::cend(types);
}

/**
 * Helper function to perform a dynamic cast from a base pointer to a derived
 * pointer.
 *
 * This uses custom RTTI and requires that the derived object has a static
 * bool is(S*) method which can tell whether requested object is of the
 * correct type.
 *
 * @param obj
 *   Base unique_ptr to cast.
 *
 * @returns
 *   A derived pointer to the supplied object if the cast is valid, otherwise
 *   nullptr.
 */
template<
    class Derived,
    class Base,
    typename = std::enable_if_t<std::is_base_of<Base, Derived>::value>>
inline Derived* dyn_cast(const std::unique_ptr<Base> &obj)
{
    return dyn_cast<Derived>(obj.get());
}

/**
 * Helper function to perform a dynamic cast from a base pointer to a derived
 * pointer.
 *
 * This uses custom RTTI and requires that the derived object has a static
 * bool is(S*) method which can tell whether requested object is of the
 * correct type.
 *
 * @param obj
 *   Base raw pointer to cast.
 *
 * @returns
 *   A derived pointer to the supplied object if the cast is valid, otherwise
 *   nullptr.
 */
template<
    class Derived,
    class Base,
    typename = std::enable_if_t<std::is_base_of<Base, Derived>::value>>
inline Derived* dyn_cast(Base *obj)
{
    return is<Derived>(obj)
        ? static_cast<Derived*>(obj)
        : nullptr;
}

}

