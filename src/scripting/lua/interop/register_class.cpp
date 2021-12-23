////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "scripting/lua/interop/register_class.h"

extern "C"
{
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include "core/error_handling.h"
#include "scripting/lua/interop/class_interop.h"

namespace
{

/**
 * Helper function to register a function with lua.
 *
 * @param state
 *   Lua state object.
 *
 * @param lua_name
 *   Name to register function with.
 *
 * @param function
 *   Function to register with name.
 */
void register_function(lua_State *state, const std::string &lua_name, iris::interop::lua::LuaInteropFunction function)
{
    auto raw_func = function.target<int (*)(lua_State *)>();
    iris::ensure(*raw_func != nullptr, "could not get raw function pointer");

    ::lua_pushcfunction(state, *raw_func);
    ::lua_setfield(state, -2, lua_name.c_str());
}

}

namespace iris::interop::lua
{

void register_class(lua_State *state, const ClassInterop &class_interop)
{
    // must have ctor and stor
    ensure(class_interop.constructor != nullptr, "missing class constructor");
    ensure(class_interop.destructor != nullptr, "missing class destructor");

    // register class
    lua_register(state, class_interop.name.c_str(), *class_interop.constructor.target<int (*)(lua_State *)>());
    ::luaL_newmetatable(state, class_interop.name.c_str());

    // register dtor
    register_function(state, "__gc", class_interop.destructor);

    // we don't support index
    ::lua_pushvalue(state, -1);
    ::lua_setfield(state, -2, "__index");

    // register all supplied members
    for (const auto &[name, function] : class_interop.members)
    {
        ensure(function != nullptr, "missing function");
        register_function(state, name, function);
    }

    ::lua_pop(state, 1);
}

}
