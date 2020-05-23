#pragma once

#include "core/root.h"
#include "log/log_level.h"
#include "log/logger.h"

// convenient macros for logging
#define LOG_DEBUG(T, ...) eng::Root::logger().log(eng::LogLevel::DEBUG, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_INFO(T, ...) eng::Root::logger().log(eng::LogLevel::INFO, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_WARN(T, ...) eng::Root::logger().log(eng::LogLevel::WARN, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_ERROR(T, ...) eng::Root::logger().log(eng::LogLevel::ERROR, T, __FILE__, __LINE__, false, __VA_ARGS__)

// convenient macros for engine logging
#define LOG_ENGINE_DEBUG(T, ...) eng::Root::logger().log(eng::LogLevel::DEBUG, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_INFO(T, ...) eng::Root::logger().log(eng::LogLevel::INFO, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_WARN(T, ...) eng::Root::logger().log(eng::LogLevel::WARN, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_ERROR(T, ...) eng::Root::logger().log(eng::LogLevel::ERROR, T, __FILE__, __LINE__, true, __VA_ARGS__)

