////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <string>

#include "core/quaternion.h"
#include "core/vector3.h"

namespace iris
{

/**
 * Interface for script, an object which can execute code from another language embedded in the engine.
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
class Script
{
  public:
    virtual ~Script() = default;

    /**
     * Set the name of the function in the script to call.
     *
     * @param function
     *   Name of function to call.
     */
    virtual void set_function(const std::string &function) = 0;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    virtual void set_argument(bool argument) = 0;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    virtual void set_argument(std::int32_t argument) = 0;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    virtual void set_argument(float argument) = 0;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    virtual void set_argument(const char *argument) = 0;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    virtual void set_argument(const std::string &argument) = 0;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    virtual void set_argument(const Vector3 &argument) = 0;

    /**
     * Set an argument for the executing function.
     *
     * @param argument
     *   Argument for function.
     */
    virtual void set_argument(const Quaternion &argument) = 0;

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
    virtual void execute(std::uint32_t num_args, std::uint32_t num_results) = 0;

    /**
     * Get a result from the executed function.
     *
     * @param result
     *   Out parameter for result.
     */
    virtual void get_result(bool &result) = 0;

    /**
     * Get a result from the executed function.
     *
     * @param result
     *   Out parameter for result.
     */
    virtual void get_result(std::int32_t &result) = 0;

    /**
     * Get a result from the executed function.
     *
     * @param result
     *   Out parameter for result.
     */
    virtual void get_result(float &result) = 0;

    /**
     * Get a result from the executed function.
     *
     * @param result
     *   Out parameter for result.
     */
    virtual void get_result(std::string &result) = 0;

    /**
     * Get a result from the executed function.
     *
     * @param result
     *   Out parameter for result.
     */
    virtual void get_result(Vector3 &result) = 0;

    /**
     * Get a result from the executed function.
     *
     * @param result
     *   Out parameter for result.
     */
    virtual void get_result(Quaternion &result) = 0;
};

}
