#pragma once

#include "core/root.h"
#include "log/log_level.h"
#include "log/logger.h"

// convenient macros for logging
#define LOG_DEBUG(T, ...) iris::Root::logger().log(iris::LogLevel::DEBUG, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_INFO(T, ...) iris::Root::logger().log(iris::LogLevel::INFO, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_WARN(T, ...) iris::Root::logger().log(iris::LogLevel::WARN, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_ERROR(T, ...) iris::Root::logger().log(iris::LogLevel::ERROR, T, __FILE__, __LINE__, false, __VA_ARGS__)

// convenient macros for engine logging
#define LOG_ENGINE_DEBUG(T, ...) iris::Root::logger().log(iris::LogLevel::DEBUG, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_INFO(T, ...) iris::Root::logger().log(iris::LogLevel::INFO, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_WARN(T, ...) iris::Root::logger().log(iris::LogLevel::WARN, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_ERROR(T, ...) iris::Root::logger().log(iris::LogLevel::ERROR, T, __FILE__, __LINE__, true, __VA_ARGS__)

