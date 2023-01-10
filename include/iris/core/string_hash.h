////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.str)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace iris
{

/**
 * A hash object fot std::unordered_map/set with a std::string key that enabled heterogeneous operations.
 */
struct StringHash
{
    using is_transparent = void;

    [[nodiscard]] size_t operator()(const char *str) const
    {
        return std::hash<std::string_view>{}(str);
    }

    [[nodiscard]] size_t operator()(std::string_view str) const
    {
        return std::hash<std::string_view>{}(str);
    }

    [[nodiscard]] size_t operator()(const std::string &str) const
    {
        return std::hash<std::string>{}(str);
    }
};

}