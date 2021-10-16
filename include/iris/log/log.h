////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "log/log_level.h"
#include "log/logger.h"

#if !defined(NDEBUG)

// convenient macros for logging
#define LOG_DEBUG(T, ...) iris::Logger::instance().log(iris::LogLevel::DEBUG, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_INFO(T, ...) iris::Logger::instance().log(iris::LogLevel::INFO, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_WARN(T, ...) iris::Logger::instance().log(iris::LogLevel::WARN, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_ERROR(T, ...) iris::Logger::instance().log(iris::LogLevel::ERR, T, __FILE__, __LINE__, false, __VA_ARGS__)

// convenient macros for engine logging
#define LOG_ENGINE_DEBUG(T, ...)                                                                                       \
    iris::Logger::instance().log(iris::LogLevel::DEBUG, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_INFO(T, ...)                                                                                        \
    iris::Logger::instance().log(iris::LogLevel::INFO, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_WARN(T, ...)                                                                                        \
    iris::Logger::instance().log(iris::LogLevel::WARN, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_ERROR(T, ...)                                                                                       \
    iris::Logger::instance().log(iris::LogLevel::ERR, T, __FILE__, __LINE__, true, __VA_ARGS__)

#else

// convenient macros for logging
#define LOG_DEBUG(T, ...)
#define LOG_INFO(T, ...)
#define LOG_WARN(T, ...)
#define LOG_ERROR(T, ...)

// convenient macros for engine logging
#define LOG_ENGINE_DEBUG(T, ...)
#define LOG_ENGINE_INFO(T, ...)
#define LOG_ENGINE_WARN(T, ...)
#define LOG_ENGINE_ERROR(T, ...)

#endif
