#pragma once

#include "log/log_level.hpp"
#include "log/logger.hpp"

// convenient macros for logging
#define LOG_DEBUG(T, ...) eng::logger::instance().log(eng::log_level::DEBUG, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_INFO(T, ...) eng::logger::instance().log(eng::log_level::INFO, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_WARN(T, ...) eng::logger::instance().log(eng::log_level::WARN, T, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_ERROR(T, ...) eng::logger::instance().log(eng::log_level::ERROR, T, __FILE__, __LINE__, false, __VA_ARGS__)

// convenient macros for engine logging
#define LOG_ENGINE_DEBUG(T, ...) eng::logger::instance().log(eng::log_level::DEBUG, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_INFO(T, ...) eng::logger::instance().log(eng::log_level::INFO, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_WARN(T, ...) eng::logger::instance().log(eng::log_level::WARN, T, __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ENGINE_ERROR(T, ...) eng::logger::instance().log(eng::log_level::ERROR, T, __FILE__, __LINE__, true, __VA_ARGS__)

