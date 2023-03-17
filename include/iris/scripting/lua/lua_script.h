////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "core/quaternion.h"
#include "core/resource_manager.h"
#include "core/vector3.h"
#include "scripting/script.h"

namespace iris
{

/**
 * Implementation of Script for Lua.
 *
 * Note that this interface is deliberately verbose. It's effectively a state machine where you:
 * - set the function you want to call
 * - set the arguments (on order)
 * - call execute (with the number of arguments and results)
 * - get all the results (in reverse order)
 *
 * Whilst flexible this is error prone, the preferred way to execute a script is to use the ScriptRunner class, which
 * handles all the complexity and provides a much simpler interface.
 */
class LuaScript : public Script
{
  public:
    /**
     * Construct a new LuaScript.
     *
     * @param source
     *   The source for the lua script.
     */
    explicit LuaScript(const std::string &source);

    /**
     * Construct a new LuaScript.
     *
     * @param file
     *   File to load script from (via ResourceLoader).
     *
     * @param
     *   Tag to indicate script should be loaded from a file.
     */
    LuaScript(ResourceManager &resource_manager, const std::string &file);

    ~LuaScript() override;

    LuaScript(const LuaScript &) = delete;
    LuaScript &operator=(const LuaScript &) = delete;

    /**
     * Set the name of the function in the script to call.
     *
     * @param function
     *   Name of function to call.
     */
    void set_function(const std::string &function) override;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    void set_argument(bool argument) override;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    void set_argument(std::int32_t argument) override;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    void set_argument(float argument) override;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    void set_argument(const char *argument) override;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    void set_argument(const std::string &argument) override;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    void set_argument(const Vector3 &argument) override;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    void set_argument(const Quaternion &argument) override;

    /**
     * Execute the set function.
     *
     * Note it is undefined to call this method without first correctly setting the function name and arguments.
     *
     * @param num_args
     *   Number of arguments to pass to the function.
     *
     * @param num_results
     *   Number if results expected from the script.
     */
    void execute(std::uint32_t num_args, std::uint32_t num_results) override;

    /**
     * Get a result from the executed function.
     *
     * @param result
     *   Out parameter for result.
     */
    void get_result(bool &result) override;

    /**
     * Get a result from the executed function.
     *
     * @param result
     *   Out parameter for result.
     */
    void get_result(std::int32_t &result) override;

    /**
     * Get a result from the executed function.
     *
     * @param result
     *   Out parameter for result.
     */
    void get_result(float &result) override;

    /**
     * Get a result from the executed function.
     *
     * @param result
     *   Out parameter for result.
     */
    void get_result(std::string &result) override;

    /**
     * Get a result from the executed function.
     *
     * @param result
     *   Out parameter for result.
     */
    void get_result(Vector3 &result) override;

    /**
     * Get a result from the executed function.
     *
     * @param result
     *   Out parameter for result.
     */
    void get_result(Quaternion &result) override;

  private:
    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
