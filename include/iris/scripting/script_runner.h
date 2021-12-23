////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <tuple>
#include <type_traits>

#include "scripting/script.h"

namespace script_runner_impl
{

/**
 * Helper function to get results of script.
 *
 * @param script
 *   Script to get results from.
 *
 * @param result
 *   Tuple to store results in.
 */
template <int I, int M, class T>
inline void get_results(iris::Script *script, T &result)
{
    script->get_result(std::get<M - I - 1>(result));

    if constexpr (I != M - 1)
    {
        get_results<I + 1, M>(script, result);
    }
}

/**
 * Base case no arguments specialisation.
 */
inline void handle_args(iris::Script *)
{
}

/**
 * Base case single arguments specialisation.
 *
 * @param script
 *   Script to set arguments for.
 *
 * @param head
 *   Head of variadic list.
 *
 */
template <class Head>
inline void handle_args(iris::Script *script, Head head)
{
    script->set_argument(head);
}

/**
 * Template function to recursively set each argument in a variadic list.
 *
 * @param script
 *   Script to set arguments for.
 *
 * @param head
 *   Head of variadic list.
 *
 * @param tail
 *   Rest of variadic list.
 */
template <class Head, class... Tail>
inline void handle_args(iris::Script *script, Head head, Tail... tail)
{
    handle_args(script, head);
    handle_args(script, tail...);
}

}

namespace iris
{

// Base struct for deducing return type of ScriptRunner::execute.
template <class... R>
struct ReturnHelper;

// Specialisation - no arguments then type is void.
template <>
struct ReturnHelper<>
{
    using type = void;
};

// Specialisation - single argument then type is that argument
template <class R>
struct ReturnHelper<R>
{
    using type = R;
};

// Specialisation - multiple argumetns then type is tuple of all arguments
template <class... R>
struct ReturnHelper
{
    using type = std::tuple<R...>;
};

// convenience alias
template <class... R>
using return_type_t = typename ReturnHelper<R...>::type;

/**
 * This is a convenience class for executing a Script object. It abstracts away the need for calling all the individual
 * functions in Script.
 */
class ScriptRunner
{
  public:
    /**
     * Construct a new ScriptRunner.
     *
     * @param script
     *   Script to run.
     */
    ScriptRunner(std::unique_ptr<Script> script)
        : script_(std::move(script))
    {
    }

    ScriptRunner(const ScriptRunner &) = delete;
    ScriptRunner &operator=(const ScriptRunner &) = delete;

    /**
     * Execute a function in the script with the supplied arguments.
     *
     * The supplied template parameters should be the expected result types.
     *
     * @param function
     *   Name of function to execute.
     *
     * @param args
     *   Arguments to function.
     *
     * @return
     *   Either void, a single type or tuple of multiple types depending on supplied template parameters.
     */
    template <class... R, class... Args>
    return_type_t<R...> execute(const std::string &function, Args &&...args)
    {
        if constexpr (sizeof...(R) == 0)
        {
            // no return type (a void function)
            std::tuple<> result;
            do_execute(function, result, std::forward<Args>(args)...);
        }
        else if constexpr (sizeof...(R) == 1)
        {
            // single return type

            // create a result tuple of just the single type and we will extract it at the end
            std::tuple<return_type_t<R...>> result;
            do_execute(function, result, std::forward<Args>(args)...);
            return std::get<0>(result);
        }
        else
        {
            // multiple return types
            return_type_t<R...> result;
            do_execute(function, result, std::forward<Args>(args)...);
            return result;
        }
    }

  private:
    /**
     * Do the execution of the script.
     *
     * @param function
     *   Name of function to execute.
     *
     * @param result
     *   Out parameter for script results. Must be a tuple.
     *
     * @param args
     *   Arguments to function.
     */
    template <class... Args, class T>
    void do_execute(const std::string &function, T &result, Args &&...args)
    {
        // set function name
        script_->set_function(function);

        // set function arguments
        script_runner_impl::handle_args(script_.get(), std::forward<Args>(args)...);

        // execute function
        script_->execute(sizeof...(Args), std::tuple_size_v<T>);

        // get results (if any)
        if constexpr (std::tuple_size_v<T> != 0)
        {
            script_runner_impl::get_results<0, std::tuple_size_v<T>>(script_.get(), result);
        }
    }

  private:
    /** Script object to run. */
    std::unique_ptr<Script> script_;
};

}
