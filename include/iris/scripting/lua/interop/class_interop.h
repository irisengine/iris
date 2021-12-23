////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>
#include <map>
#include <string>

struct lua_State;

namespace iris::interop::lua
{

using LuaInteropFunction = std::function<int(lua_State *)>;

/**
 * Struct encapsulating all the data needed to register a class for lua interop.
 */
struct ClassInterop
{
    /** Name of class. */
    std::string name;

    /** Constructor function. */
    LuaInteropFunction constructor;

    /** Constructor function. */
    LuaInteropFunction destructor;

    /** Collection of named member functions. */
    std::map<std::string, LuaInteropFunction> members;
};

}
