////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "scripting/lua/interop/class_interop.h"

struct lua_State;

namespace iris::interop::lua
{

/**
 * Register a class for lua interop.
 *
 * @param state
 *   Lua state object.
 *
 * @param class_interop
 *   Class data to register.
 */
void register_class(lua_State *state, const ClassInterop &class_interop);

}
