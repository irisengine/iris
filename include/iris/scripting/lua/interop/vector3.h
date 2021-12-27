////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <sstream>
#include <string>

#include "core/error_handling.h"
#include "core/vector3.h"
#include "scripting/lua/interop/class_interop.h"

struct lua_State;

namespace iris::interop::lua
{

void vector3_construct_raw(lua_State *state, const Vector3 &vec);

Vector3 **get_vector3(lua_State *state);

// these functions all represent the various supported interop functionality

int vector3_constructor(lua_State *state);

int vector3_destructor(lua_State *state);

int vector3_x(lua_State *state);

int vector3_set_x(lua_State *state);

int vector3_y(lua_State *state);

int vector3_set_y(lua_State *state);

int vector3_z(lua_State *state);

int vector3_set_z(lua_State *state);

int vector3_to_string(lua_State *state);

int vector3_equality(lua_State *state);

int vector3_negate(lua_State *state);

int vector3_add(lua_State *state);

int vector3_sub(lua_State *state);

int vector3_mul(lua_State *state);

int vector3_dot(lua_State *state);

int vector3_cross(lua_State *state);

int vector3_normalise(lua_State *state);

int vector3_magnitude(lua_State *state);

int vector3_lerp(lua_State *state);

static const ClassInterop vector3_class_interop{
    .name = "Vector3",
    .constructor = vector3_constructor,
    .destructor = vector3_destructor,
    .members = {
        {"__tostring", vector3_to_string},
        {"__eq", vector3_to_string},
        {"__unm", vector3_negate},
        {"__add", vector3_add},
        {"__sub", vector3_sub},
        {"__mul", vector3_mul},
        {"x", vector3_x},
        {"set_x", vector3_set_x},
        {"y", vector3_y},
        {"set_y", vector3_set_y},
        {"z", vector3_z},
        {"set_z", vector3_set_z},
        {"dot", vector3_dot},
        {"cross", vector3_cross},
        {"normalise", vector3_normalise},
        {"magnitude", vector3_magnitude},
        {"lerp", vector3_lerp}}};

}
