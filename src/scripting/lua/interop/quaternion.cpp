////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "scripting/lua/interop/quaternion.h"

#include <sstream>
#include <string>

extern "C"
{
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include "core/error_handling.h"
#include "core/object_pool.h"
#include "core/quaternion.h"
#include "core/vector3.h"

namespace iris::interop::lua
{

static ObjectPool<Quaternion> quaternion_pool;

void quaternion_construct_raw(lua_State *state, const Quaternion &quaternion)
{
    auto **user_data = reinterpret_cast<Quaternion **>(::lua_newuserdata(state, sizeof(Quaternion *)));
    *user_data = quaternion_pool.next(quaternion);
    luaL_setmetatable(state, "Quaternion");
}

Quaternion **get_quaternion(lua_State *state)
{
    auto *user_data = luaL_checkudata(state, 1, "Quaternion");
    expect(user_data != nullptr, "incorrect user data");

    return reinterpret_cast<Quaternion **>(user_data);
}

int quaternion_constructor(lua_State *state)
{
    const auto arg1 = static_cast<float>(::luaL_checknumber(state, 1));
    const auto arg2 = static_cast<float>(::luaL_checknumber(state, 2));
    const auto arg3 = static_cast<float>(::luaL_checknumber(state, 3));
    const auto arg4 = static_cast<float>(::luaL_checknumber(state, 4));
    auto **user_data = reinterpret_cast<Quaternion **>(::lua_newuserdata(state, sizeof(Quaternion *)));
    *user_data = quaternion_pool.next(arg1, arg2, arg3, arg4);
    luaL_setmetatable(state, "Quaternion");

    return 1;
}

int quaternion_destructor(lua_State *state)
{
    quaternion_pool.release(*get_quaternion(state));
    return 0;
}

int quaternion_x(lua_State *state)
{
    auto **q = get_quaternion(state);

    ::lua_pushnumber(state, (*q)->x);
    return 1;
}

int quaternion_set_x(lua_State *state)
{
    auto **q = get_quaternion(state);

    (*q)->x = static_cast<float>(::luaL_checknumber(state, 2));

    return 0;
}

int quaternion_y(lua_State *state)
{
    auto **q = get_quaternion(state);

    ::lua_pushnumber(state, (*q)->y);
    return 1;
}

int quaternion_set_y(lua_State *state)
{
    auto **q = get_quaternion(state);

    (*q)->y = static_cast<float>(::luaL_checknumber(state, 2));

    return 0;
}

int quaternion_z(lua_State *state)
{
    auto **q = get_quaternion(state);

    ::lua_pushnumber(state, (*q)->z);
    return 1;
}

int quaternion_set_z(lua_State *state)
{
    auto **q = get_quaternion(state);

    (*q)->z = static_cast<float>(::luaL_checknumber(state, 2));

    return 0;
}

int quaternion_w(lua_State *state)
{
    auto **q = get_quaternion(state);

    ::lua_pushnumber(state, (*q)->w);
    return 1;
}

int quaternion_set_w(lua_State *state)
{
    auto **q = get_quaternion(state);

    (*q)->w = static_cast<float>(::luaL_checknumber(state, 2));

    return 0;
}

int quaternion_to_string(lua_State *state)
{
    auto **q = get_quaternion(state);

    std::stringstream strm{};
    strm << **q;
    const auto str = strm.str();

    ::lua_pushstring(state, str.c_str());

    return 1;
}

int equality_equality(lua_State *state)
{
    auto *user_data1 = luaL_checkudata(state, 1, "Quaternion");
    expect(user_data1 != nullptr, "incorrect user data");
    auto **q1 = reinterpret_cast<Quaternion **>(user_data1);

    auto *user_data2 = luaL_checkudata(state, 2, "Quaternion");
    expect(user_data2 != nullptr, "incorrect user data");
    auto **q2 = reinterpret_cast<Quaternion **>(user_data2);

    ::lua_pushboolean(state, **q1 == **q2);

    return 1;
}

int quaternion_negate(lua_State *state)
{
    auto **q = get_quaternion(state);

    quaternion_construct_raw(state, -**q);

    return 1;
}

int quaternion_add(lua_State *state)
{
    auto *user_data1 = luaL_checkudata(state, 2, "Quaternion");
    expect(user_data1 != nullptr, "incorrect user data");
    auto **q1 = reinterpret_cast<Quaternion **>(user_data1);

    auto *user_data2 = luaL_checkudata(state, 1, "Quaternion");
    expect(user_data2 != nullptr, "incorrect user data");
    auto **q2 = reinterpret_cast<Quaternion **>(user_data2);

    quaternion_construct_raw(state, **q1 + **q2);

    return 1;
}

int quaternion_sub(lua_State *state)
{
    auto *user_data1 = luaL_checkudata(state, 1, "Quaternion");
    expect(user_data1 != nullptr, "incorrect user data");
    auto **q1 = reinterpret_cast<Quaternion **>(user_data1);

    auto *user_data2 = luaL_checkudata(state, 2, "Quaternion");
    expect(user_data2 != nullptr, "incorrect user data");
    auto **q2 = reinterpret_cast<Quaternion **>(user_data2);

    quaternion_construct_raw(state, **q1 - **q2);

    return 1;
}

int quaternion_mul(lua_State *state)
{
    auto *user_data1 = luaL_checkudata(state, 1, "Quaternion");
    expect(user_data1 != nullptr, "incorrect user data");
    auto **q1 = reinterpret_cast<Quaternion **>(user_data1);

    auto *user_data2 = luaL_checkudata(state, 2, "Quaternion");
    expect(user_data2 != nullptr, "incorrect user data");
    auto **q2 = reinterpret_cast<Quaternion **>(user_data2);

    quaternion_construct_raw(state, **q1 * **q2);

    return 1;
}

int quaternion_dot(lua_State *state)
{
    auto *user_data1 = luaL_checkudata(state, 1, "Quaternion");
    expect(user_data1 != nullptr, "incorrect user data");
    auto **q1 = reinterpret_cast<Quaternion **>(user_data1);

    auto *user_data2 = luaL_checkudata(state, 2, "Quaternion");
    expect(user_data2 != nullptr, "incorrect user data");
    auto **q2 = reinterpret_cast<Quaternion **>(user_data2);

    ::lua_pushnumber(state, (*q1)->dot(**q2));

    return 1;
}

int quaternion_slerp(lua_State *state)
{
    auto *user_data1 = luaL_checkudata(state, 1, "Quaternion");
    expect(user_data1 != nullptr, "incorrect user data");
    auto **q1 = reinterpret_cast<Quaternion **>(user_data1);

    auto *user_data2 = luaL_checkudata(state, 2, "Quaternion");
    expect(user_data2 != nullptr, "incorrect user data");
    auto **q2 = reinterpret_cast<Quaternion **>(user_data2);

    const auto amount = static_cast<float>(::luaL_checknumber(state, 3));

    auto tmp_q = **q1;
    tmp_q.slerp(**q2, amount);

    quaternion_construct_raw(state, tmp_q);

    return 1;
}

int quaternion_normalise(lua_State *state)
{
    auto **q = get_quaternion(state);

    auto tmp_q = **q;
    tmp_q.normalise();

    quaternion_construct_raw(state, tmp_q);

    return 1;
}

}
