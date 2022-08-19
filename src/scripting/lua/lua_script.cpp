////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "scripting/lua/lua_script.h"

#include <cstdint>
#include <memory>
#include <sstream>
#include <string>

extern "C"
{
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include "core/auto_release.h"
#include "core/error_handling.h"
#include "core/resource_loader.h"
#include "core/vector3.h"
#include "scripting/lua/interop/quaternion.h"
#include "scripting/lua/interop/register_class.h"
#include "scripting/lua/interop/vector3.h"

namespace
{

/**
 * Helper function to create a lua state from a script string.
 *
 * @param source
 *   Lua script source.
 *
 * @return
 *   Lua state object.
 */
iris::AutoRelease<lua_State *, nullptr> create_lua_state(const std::string &source)
{
    auto *state = ::luaL_newstate();
    iris::ensure(state != nullptr, "could not create lua state");

    // register standard library and engine custom classes
    ::luaL_openlibs(state);
    iris::interop::lua::register_class(state, iris::interop::lua::vector3_class_interop);
    iris::interop::lua::register_class(state, iris::interop::lua::quaternion_class_interop);

    // try and load the script
    if (::luaL_loadstring(state, source.c_str()) == LUA_OK)
    {
        if (::lua_pcall(state, 0, 0, 0) == LUA_OK)
        {
            ::lua_pop(state, ::lua_gettop(state));
        }
    }
    else
    {
        // if the script failed to load then get the error string from the lua stack and throw it
        const auto error_msg = ::lua_tostring(state, -1);

        std::stringstream strm{};
        strm << "Execution of function failed: " << error_msg;

        throw iris::Exception(strm.str());
    }

    return {state, ::lua_close};
}

}

namespace iris
{

struct LuaScript::implementation
{
    AutoRelease<lua_State *, nullptr> state;
};

LuaScript::LuaScript(const std::string &source)
    : Script()
    , impl_(std::make_unique<implementation>())
{
    impl_->state = create_lua_state(source);
}

LuaScript::LuaScript(const std::string &file, LoadFile)
    : Script()
    , impl_(std::make_unique<implementation>())
{
    const auto script_data = ResourceLoader::instance().load(file);
    const std::string source(reinterpret_cast<const char *>(script_data.data()), script_data.size());

    impl_->state = create_lua_state(source);
}

LuaScript::~LuaScript() = default;

void LuaScript::set_argument(bool argument)
{
    ::lua_pushboolean(impl_->state, argument);
}

void LuaScript::set_function(const std::string &function)
{
    ensure(::lua_getglobal(impl_->state, function.c_str()) == LUA_TFUNCTION, "missing function");
}

void LuaScript::set_argument(std::int32_t argument)
{
    ::lua_pushinteger(impl_->state, argument);
}

void LuaScript::set_argument(float argument)
{
    ::lua_pushnumber(impl_->state, argument);
}

void LuaScript::set_argument(const char *argument)
{
    ::lua_pushstring(impl_->state, argument);
}

void LuaScript::set_argument(const std::string &argument)
{
    set_argument(argument.c_str());
}

void LuaScript::set_argument(const Vector3 &argument)
{
    interop::lua::vector3_construct_raw(impl_->state, argument);
}

void LuaScript::set_argument(const Quaternion &argument)
{
    interop::lua::quaternion_construct_raw(impl_->state, argument);
}

void LuaScript::execute(std::uint32_t num_args, std::uint32_t num_results)
{
    if (::lua_pcall(impl_->state, num_args, num_results, 0) != LUA_OK)
    {
        std::string error_msg;
        get_result(error_msg);

        std::stringstream strm{};
        strm << "Execution of function failed: " << error_msg;

        throw Exception(strm.str());
    }
}

void LuaScript::get_result(bool &result)
{
    ensure(lua_isboolean(impl_->state, -1) == 1, "incorrect result type");

    result = ::lua_toboolean(impl_->state, -1);
    ::lua_pop(impl_->state, 1);
}

void LuaScript::get_result(std::int32_t &result)
{
    ensure(lua_isinteger(impl_->state, -1) == 1, "incorrect result type");

    result = static_cast<std::int32_t>(::lua_tointeger(impl_->state, -1));
    ::lua_pop(impl_->state, 1);
}

void LuaScript::get_result(float &result)
{
    ensure(lua_isnumber(impl_->state, -1) == 1, "incorrect result type");

    result = static_cast<float>(::lua_tonumber(impl_->state, -1));
    ::lua_pop(impl_->state, 1);
}

void LuaScript::get_result(std::string &result)
{
    ensure(lua_isstring(impl_->state, -1) == 1, "incorrect result type");

    result = ::lua_tostring(impl_->state, -1);
    ::lua_pop(impl_->state, 1);
}

void LuaScript::get_result(Vector3 &result)
{
    auto **vector = interop::lua::get_vector3(impl_->state);
    result = **vector;

    ::lua_pop(impl_->state, 1);
}

void LuaScript::get_result(Quaternion &result)
{
    auto **q = interop::lua::get_quaternion(impl_->state);
    result = **q;

    ::lua_pop(impl_->state, 1);
}

}
