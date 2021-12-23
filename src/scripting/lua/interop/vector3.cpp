
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
#include "core/vector3.h"

namespace iris::interop::lua
{

static ObjectPool<Vector3> vector3_pool;

void vector3_construct_raw(lua_State *state, const Vector3 &vec)
{
    auto **user_data = reinterpret_cast<Vector3 **>(::lua_newuserdata(state, sizeof(Vector3 *)));
    *user_data = vector3_pool.next(vec);
    luaL_setmetatable(state, "Vector3");
}

Vector3 **get_vector3(lua_State *state)
{
    auto *user_data = luaL_checkudata(state, 1, "Vector3");
    expect(user_data != nullptr, "incorrect user data");

    return reinterpret_cast<Vector3 **>(user_data);
}

int vector3_constructor(lua_State *state)
{
    const auto arg1 = static_cast<float>(::luaL_checknumber(state, 1));
    const auto arg2 = static_cast<float>(::luaL_checknumber(state, 2));
    const auto arg3 = static_cast<float>(::luaL_checknumber(state, 3));
    auto **user_data = reinterpret_cast<Vector3 **>(::lua_newuserdata(state, sizeof(Vector3 *)));
    *user_data = vector3_pool.next(arg1, arg2, arg3);
    luaL_setmetatable(state, "Vector3");

    return 1;
}

int vector3_destructor(lua_State *state)
{
    vector3_pool.release(*get_vector3(state));
    return 0;
}

int vector3_x(lua_State *state)
{
    auto **vector = get_vector3(state);

    ::lua_pushnumber(state, (*vector)->x);
    return 1;
}

int vector3_set_x(lua_State *state)
{
    auto **vector = get_vector3(state);

    (*vector)->x = ::luaL_checknumber(state, 2);

    return 0;
}

int vector3_y(lua_State *state)
{
    auto **vector = get_vector3(state);

    ::lua_pushnumber(state, (*vector)->y);
    return 1;
}

int vector3_set_y(lua_State *state)
{
    auto **vector = get_vector3(state);

    (*vector)->y = ::luaL_checknumber(state, 2);

    return 0;
}

int vector3_z(lua_State *state)
{
    auto **vector = get_vector3(state);

    ::lua_pushnumber(state, (*vector)->z);
    return 1;
}

int vector3_set_z(lua_State *state)
{
    auto **vector = get_vector3(state);

    (*vector)->z = ::luaL_checknumber(state, 2);

    return 0;
}

int vector3_to_string(lua_State *state)
{
    auto **vector = get_vector3(state);

    std::stringstream strm{};
    strm << **vector;
    const auto str = strm.str();

    ::lua_pushstring(state, str.c_str());

    return 1;
}

int vector3_equality(lua_State *state)
{
    auto *user_data1 = luaL_checkudata(state, 1, "Vector3");
    expect(user_data1 != nullptr, "incorrect user data");
    auto **vec1 = reinterpret_cast<Vector3 **>(user_data1);

    auto *user_data2 = luaL_checkudata(state, 2, "Vector3");
    expect(user_data2 != nullptr, "incorrect user data");
    auto **vec2 = reinterpret_cast<Vector3 **>(user_data2);

    ::lua_pushboolean(state, **vec1 == **vec2);

    return 1;
}

int vector3_negate(lua_State *state)
{
    auto **vector = get_vector3(state);

    vector3_construct_raw(state, -**vector);

    return 1;
}

int vector3_add(lua_State *state)
{
    auto *user_data1 = luaL_checkudata(state, 2, "Vector3");
    expect(user_data1 != nullptr, "incorrect user data");
    auto **vec1 = reinterpret_cast<Vector3 **>(user_data1);

    auto *user_data2 = luaL_checkudata(state, 1, "Vector3");
    expect(user_data2 != nullptr, "incorrect user data");
    auto **vec2 = reinterpret_cast<Vector3 **>(user_data2);

    vector3_construct_raw(state, **vec1 + **vec2);

    return 1;
}

int vector3_sub(lua_State *state)
{
    auto *user_data1 = luaL_checkudata(state, 1, "Vector3");
    expect(user_data1 != nullptr, "incorrect user data");
    auto **vec1 = reinterpret_cast<Vector3 **>(user_data1);

    auto *user_data2 = luaL_checkudata(state, 2, "Vector3");
    expect(user_data2 != nullptr, "incorrect user data");
    auto **vec2 = reinterpret_cast<Vector3 **>(user_data2);

    vector3_construct_raw(state, **vec1 - **vec2);

    return 1;
}

int vector3_mul(lua_State *state)
{
    auto *user_data1 = luaL_checkudata(state, 1, "Vector3");
    expect(user_data1 != nullptr, "incorrect user data");
    auto **vec1 = reinterpret_cast<Vector3 **>(user_data1);

    auto *user_data2 = luaL_checkudata(state, 2, "Vector3");
    expect(user_data2 != nullptr, "incorrect user data");
    auto **vec2 = reinterpret_cast<Vector3 **>(user_data2);

    vector3_construct_raw(state, **vec1 * **vec2);

    return 1;
}

int vector3_dot(lua_State *state)
{
    auto *user_data1 = luaL_checkudata(state, 1, "Vector3");
    expect(user_data1 != nullptr, "incorrect user data");
    auto **vec1 = reinterpret_cast<Vector3 **>(user_data1);

    auto *user_data2 = luaL_checkudata(state, 2, "Vector3");
    expect(user_data2 != nullptr, "incorrect user data");
    auto **vec2 = reinterpret_cast<Vector3 **>(user_data2);

    ::lua_pushnumber(state, (*vec1)->dot(**vec2));

    return 1;
}

int vector3_cross(lua_State *state)
{
    auto *user_data1 = luaL_checkudata(state, 1, "Vector3");
    expect(user_data1 != nullptr, "incorrect user data");
    auto **vec1 = reinterpret_cast<Vector3 **>(user_data1);

    auto *user_data2 = luaL_checkudata(state, 2, "Vector3");
    expect(user_data2 != nullptr, "incorrect user data");
    auto **vec2 = reinterpret_cast<Vector3 **>(user_data2);

    vector3_construct_raw(state, (*vec1)->cross(**vec2));

    return 1;
}

int vector3_normalise(lua_State *state)
{
    auto **vector = get_vector3(state);

    (*vector)->normalise();

    return 0;
}

int vector3_magnitude(lua_State *state)
{
    auto **vector = get_vector3(state);

    ::lua_pushnumber(state, (*vector)->magnitude());

    return 1;
}

int vector3_lerp(lua_State *state)
{
    auto *user_data1 = luaL_checkudata(state, 1, "Vector3");
    expect(user_data1 != nullptr, "incorrect user data");
    auto **vec1 = reinterpret_cast<Vector3 **>(user_data1);

    auto *user_data2 = luaL_checkudata(state, 2, "Vector3");
    expect(user_data2 != nullptr, "incorrect user data");
    auto **vec2 = reinterpret_cast<Vector3 **>(user_data2);

    const auto amount = static_cast<float>(::luaL_checknumber(state, 3));

    (*vec1)->lerp(**vec2, amount);

    return 0;
}

}
