////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

#if !defined(__clang__)
#include <source_location>
#endif

#if defined(IRIS_PLATFORM_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "core/auto_release.h"
#include "core/exception.h"

namespace iris
{

namespace impl
{

/**
 * Helper function to check an assertion and, if it's false, either log and
 * abort or throw an exception.
 *
 * @param assertion
 *   The assertion to check.
 *
 * @param message
 *   A user supplied message to either log (if abort) or throw with.
 *
 * @param line
 *   The source code line where the check was called from.
 *
 * @param file_name
 *   The file name where the check was called from.
 *
 * @param function_name
 *   The function where the check was called from.
 *
 * @param drop_mic
 *   If true then log and abort on assertion failure, else throw exception.
 */
inline void check_and_handle(
    bool assertion,
    std::string_view message,
    int line,
    const char *file_name,
    const char *function_name,
    bool drop_mic)
{
    if (!assertion)
    {
        std::stringstream strm{};

        // combine the user supplied message with the source location
        strm << message << " -> " << file_name << "(" << line << ") `" << function_name << "`";

        if (drop_mic)
        {
            // note we don't use the logging framework here as we don't know
            // how it's been configured, safest just to write to stderr
            std::cerr << strm.str() << std::endl;
            std::abort();
        }
        else
        {
            throw Exception(strm.str());
        }
    }
}

}

// define a platform specific macro that will break to an attached debugger
// this is useful if we cannot set a breakpoint in an IDE, for example as of
// writing VSCode cannot set a breakpoint in an objc++ (.mm) file
#if defined(IRIS_PLATFORM_WIN32)
#define IRIS_DEBUG_BREAK()                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        DebugBreak();                                                                                                  \
    } while (false)
#else
#if defined(IRIS_ARCH_X86_64)
#define IRIS_DEBUG_BREAK()                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        asm("int3");                                                                                                   \
    } while (false)
#elif defined(IRIS_ARCH_ARM64)
#define IRIS_DEBUG_BREAK()                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        asm("BKPT");                                                                                                   \
    } while (false)
#else
#error unsupported architecture
#endif
#endif

// here we define expect and ensure functions for error handling
// usage:
//   expect - check an invariant holds i.e. something that should always be true
//            will call abort on failure and is stripped from release builds
//   ensure - check that a precondition or postcondition holds i.e. something we
//            want to verify at runtime
//            will throw an exception on failure and is never stripped
//
// annoyingly (as of writing) clang does not support std::source_location so we
// define two versions, one which uses the standard and another which uses
// clangs builtins
//
// we define the various expect/ensure functions below for different compiles
// and debug/release versions

#if !defined(NDEBUG) || defined(IRIS_FORCE_EXPECT)

#if defined(__clang__)
/**
 * Check invariant and abort on failure.
 *
 * @param expectation
 *   The assertion to check.
 *
 * @param message
 *   A user supplied message to log before abort.
 */
inline void expect(
    bool expectation,
    std::string_view message,
    int line = __builtin_LINE(),
    const char *file_name = __builtin_FILE(),
    const char *function_name = __builtin_FUNCTION())
{
    impl::check_and_handle(expectation, message, line, file_name, function_name, true);
}

/**
 * Check invariant and abort on failure. This specialisation allows a callback
 * to determine if an error occurred. This is useful if errors are detected
 * via some global state and need checking via a function e.g. glGetError.
 *
 * @param check_error
 *   Callback to determine if an error occurred. message is passed as the
 *   argument. The return value should be an en empty optional if no error
 *   occurred or an error message otherwise.
 *
 * @param message
 *   A user supplied message to log before abort.
 */
inline void expect(
    std::function<std::optional<std::string>(std::string_view)> check_error,
    std::string_view message,
    int line = __builtin_LINE(),
    const char *file_name = __builtin_FILE(),
    const char *function_name = __builtin_FUNCTION())
{
    if (const auto final_message = check_error(message); final_message)
    {
        impl::check_and_handle(false, *final_message, line, file_name, function_name, true);
    }
}

/**
 * Check invariant and abort on failure. This specialisation checks if a
 * unique_ptr is nullptr.
 *
 * @param ptr
 *   unique_ptr to check for nullptr.
 *
 * @param message
 *   A user supplied message to log before abort.
 */
template <class T>
inline void expect(
    const std::unique_ptr<T> &ptr,
    std::string_view message,
    int line = __builtin_LINE(),
    const char *file_name = __builtin_FILE(),
    const char *function_name = __builtin_FUNCTION())
{
    impl::check_and_handle(!!ptr, message, line, file_name, function_name, true);
}

/**
 * Check invariant and abort on failure. This specialisation checks if an
 * AutoRelease manages an object.
 *
 * @param auto_release
 *   AutoRelease to check.
 *
 * @param message
 *   A user supplied message to log before abort.
 */
template <class T, T Invalid>
inline void expect(
    const AutoRelease<T, Invalid> &auto_release,
    std::string_view message,
    int line = __builtin_LINE(),
    const char *file_name = __builtin_FILE(),
    const char *function_name = __builtin_FUNCTION())
{
    impl::check_and_handle(!!auto_release, message, line, file_name, function_name, true);
}

#else

// see __clang__ defines for documentation

inline void expect(
    bool expectation,
    std::string_view message,
    std::source_location location = std::source_location::current())
{
    impl::check_and_handle(expectation, message, location.line(), location.file_name(), location.function_name(), true);
}

inline void expect(
    std::function<std::optional<std::string>(std::string_view)> check_error,
    std::string_view message,
    std::source_location location = std::source_location::current())
{
    if (const auto final_message = check_error(message); final_message)
    {
        impl::check_and_handle(
            false, *final_message, location.line(), location.file_name(), location.function_name(), true);
    }
}

template <class T>
inline void expect(
    const std::unique_ptr<T> &ptr,
    std::string_view message,
    std::source_location location = std::source_location::current())
{
    impl::check_and_handle(!!ptr, message, location.line(), location.file_name(), location.function_name(), true);
}

template <class T, T Invalid>
inline void expect(
    const AutoRelease<T, Invalid> &auto_release,
    std::string_view message,
    std::source_location location = std::source_location::current())
{
    impl::check_and_handle(
        !!auto_release, message, location.line(), location.file_name(), location.function_name(), true);
}

#endif

#else

#if defined(__clang__)

inline void expect(
    bool,
    std::string_view,
    int = __builtin_LINE(),
    const char * = __builtin_FILE(),
    const char * = __builtin_FUNCTION())
{
}

inline void expect(
    std::function<std::optional<std::string>(std::string_view)>,
    std::string_view,
    int = __builtin_LINE(),
    const char * = __builtin_FILE(),
    const char * = __builtin_FUNCTION())
{
}

template <class T>
inline void expect(
    const std::unique_ptr<T> &,
    std::string_view,
    int = __builtin_LINE(),
    const char * = __builtin_FILE(),
    const char * = __builtin_FUNCTION())
{
}

template <class T, T Invalid>
inline void expect(
    const AutoRelease<T, Invalid> &,
    std::string_view,
    int = __builtin_LINE(),
    const char * = __builtin_FILE(),
    const char * = __builtin_FUNCTION())
{
}

#else

inline void expect(bool, std::string_view, std::source_location = std::source_location::current())
{
}

inline void expect(
    std::function<std::optional<std::string>(std::string_view)>,
    std::string_view,
    std::source_location = std::source_location::current())
{
}

template <class T>
inline void expect(const std::unique_ptr<T> &, std::string_view, std::source_location = std::source_location::current())
{
}

template <class T, T Invalid>
inline void expect(
    const AutoRelease<T, Invalid> &,
    std::string_view,
    std::source_location = std::source_location::current())
{
}

#endif

#endif

#if defined(__clang__)

/**
 * Check pre/post-condition and throw on failure.
 *
 * @param expectation
 *   The assertion to check.
 *
 * @param message
 *   A user supplied message to throw.
 */
inline void ensure(
    bool expectation,
    std::string_view message,
    int line = __builtin_LINE(),
    const char *file_name = __builtin_FILE(),
    const char *function_name = __builtin_FUNCTION())
{
    impl::check_and_handle(expectation, message, line, file_name, function_name, false);
}

/**
 * Check pre/post-condition and throw on failure. This specialisation allows a
 * callback to determine if an error occurred. This is useful if errors are
 * detected via some global state and need checking via a function e.g.
 * glGetError.
 *
 * @param check_error
 *   Callback to determine if an error occurred. message is passed as the
 *   argument. The return value should be an en empty optional if no error
 *   occurred or an error message otherwise.
 *
 * @param message
 *   A user supplied message to log before abort.
 */
inline void ensure(
    std::function<std::optional<std::string>(std::string_view)> check_error,
    std::string_view message,
    int line = __builtin_LINE(),
    const char *file_name = __builtin_FILE(),
    const char *function_name = __builtin_FUNCTION())
{
    if (const auto final_message = check_error(message); final_message)
    {
        impl::check_and_handle(false, *final_message, line, file_name, function_name, false);
    }
}

/**
 * Check pre/post-condition and throw on failure. This specialisation checks if
 * a unique_ptr is nullptr.
 *
 * @param ptr
 *   unique_ptr to check for nullptr.
 *
 * @param message
 *   A user supplied message to log before abort.
 */
template <class T>
inline void ensure(
    const std::unique_ptr<T> &ptr,
    std::string_view message,
    int line = __builtin_LINE(),
    const char *file_name = __builtin_FILE(),
    const char *function_name = __builtin_FUNCTION())
{
    impl::check_and_handle(!!ptr, message, line, file_name, function_name, false);
}

/**
 * Check pre/post-condition and throw on failure. This specialisation checks if
 * an AutoRelease manages an object.
 *
 * @param auto_release
 *   AutoRelease to check.
 *
 * @param message
 *   A user supplied message to log before abort.
 */
template <class T, T Invalid>
inline void ensure(
    const AutoRelease<T, Invalid> &auto_release,
    std::string_view message,
    int line = __builtin_LINE(),
    const char *file_name = __builtin_FILE(),
    const char *function_name = __builtin_FUNCTION())
{
    impl::check_and_handle(!!auto_release, message, line, file_name, function_name, false);
}

#else

// see __clang__ defines for documentation

/**
 * Check pre-condition and throw on failure.
 *
 * @param expectation
 *   The assertion to check.
 *
 * @param message
 *   A user supplied message to throw.
 */
inline void ensure(
    bool expectation,
    std::string_view message,
    std::source_location location = std::source_location::current())
{
    impl::check_and_handle(
        expectation, message, location.line(), location.file_name(), location.function_name(), false);
}

inline void ensure(
    std::function<std::optional<std::string>(std::string_view)> check_error,
    std::string_view message,
    std::source_location location = std::source_location::current())
{
    if (const auto final_message = check_error(message); final_message)
    {
        impl::check_and_handle(
            false, *final_message, location.line(), location.file_name(), location.function_name(), false);
    }
}

template <class T>
inline void ensure(
    const std::unique_ptr<T> &ptr,
    std::string_view message,
    std::source_location location = std::source_location::current())
{
    impl::check_and_handle(!!ptr, message, location.line(), location.file_name(), location.function_name(), false);
}

template <class T, T Invalid>
inline void ensure(
    const AutoRelease<T, Invalid> &auto_release,
    std::string_view message,
    std::source_location location = std::source_location::current())
{
    impl::check_and_handle(
        !!auto_release, message, location.line(), location.file_name(), location.function_name(), false);
}

#endif

}
