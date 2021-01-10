#pragma once

#include "log/log_level.h"
#include "log/logger.h"

// convenient macros for logging
#define LOG_DEBUG(T, ...)                                                      \
    iris::Logger::instance().log(                                              \
        iris::LogLevel::DEBUG, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_INFO(T, ...)                                                       \
    iris::Logger::instance().log(                                              \
        iris::LogLevel::INFO, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_WARN(T, ...)                                                       \
    iris::Logger::instance().log(                                              \
        iris::LogLevel::WARN, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_ERROR(T, ...)                                                      \
    iris::Logger::instance().log(                                              \
        iris::LogLevel::ERR, T, __FILE__, __LINE__, false, __VA_ARGS__)

// convenient macros for engine logging
#define LOG_ENGINE_DEBUG(T, ...)                                               \
    iris::Logger::instance().log(                                              \
        iris::LogLevel::DEBUG, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_INFO(T, ...)                                                \
    iris::Logger::instance().log(                                              \
        iris::LogLevel::INFO, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_WARN(T, ...)                                                \
    iris::Logger::instance().log(                                              \
        iris::LogLevel::WARN, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_ERROR(T, ...)                                               \
    iris::Logger::instance().log(                                              \
        iris::LogLevel::ERR, T, __FILE__, __LINE__, true, __VA_ARGS__)
