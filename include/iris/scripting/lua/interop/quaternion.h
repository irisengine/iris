////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <sstream>
#include <string>

#include "core/error_handling.h"
#include "core/quaternion.h"
#include "core/vector3.h"
#include "scripting/lua/interop/class_interop.h"

struct lua_State;

namespace iris::interop::lua
{

void quaternion_construct_raw(lua_State *state, const Quaternion &quaternion);

Quaternion **get_quaternion(lua_State *state);

// these functions all represent the various supported interop functionality

int quaternion_constructor(lua_State *state);

int quaternion_destructor(lua_State *state);

int quaternion_x(lua_State *state);

int quaternion_set_x(lua_State *state);

int quaternion_y(lua_State *state);

int quaternion_set_y(lua_State *state);

int quaternion_z(lua_State *state);

int quaternion_set_z(lua_State *state);

int quaternion_w(lua_State *state);

int quaternion_set_w(lua_State *state);

int quaternion_to_string(lua_State *state);

int quaternion_equality(lua_State *state);

int quaternion_negate(lua_State *state);

int quaternion_add(lua_State *state);

int quaternion_sub(lua_State *state);

int quaternion_mul(lua_State *state);

int quaternion_dot(lua_State *state);

int quaternion_slerp(lua_State *state);

int quaternion_normalise(lua_State *state);

static const ClassInterop quaternion_class_interop{
    .name = "Quaternion",
    .constructor = quaternion_constructor,
    .destructor = quaternion_destructor,
    .members = {
        {"__tostring", quaternion_to_string},
        {"__eq", quaternion_to_string},
        {"__unm", quaternion_negate},
        {"__add", quaternion_add},
        {"__sub", quaternion_sub},
        {"__mul", quaternion_mul},
        {"x", quaternion_x},
        {"set_x", quaternion_set_x},
        {"y", quaternion_y},
        {"set_y", quaternion_set_y},
        {"z", quaternion_z},
        {"set_z", quaternion_set_z},
        {"w", quaternion_w},
        {"set_w", quaternion_set_w},
        {"dot", quaternion_dot},
        {"slerp", quaternion_slerp},
        {"normalise", quaternion_normalise}}};

}
