////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>
#include <iterator>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>

#include "log/colour_formatter.h"
#include "log/log_level.h"
#include "log/stdout_outputter.h"

namespace iris
{

namespace detail
{

/**
 * Helper function to write a string to a stream. Writes from the supplied
 * position to the first occurrence of '{}' (which is replaced by supplied
 * object). If '{}' is not found then nothing is written.
 *
 * @param format_str
 *   The format string to write.
 *
 * @param obj
 *   The object to write.
 *
 * @param pos
 *   [in/out] On function enter this is the position to start searching in
 *   format_str for '{}'. On exit in the position in format_str after the '{}'.
 *   If no '{}' is found in the string then it is std::string::npos.
 *
 * @param strm
 *   The stream to write to.
 */
template <class T>
void format(
    const std::string &format_str,
    T &&obj,
    std::size_t &pos,
    std::stringstream &strm)
{
    static const std::string format_pattern{"{}"};

    const auto current_pos = pos;

    // find the pattern
    pos = format_str.find(format_pattern, pos);
    if (pos != std::string::npos)
    {
        // write the string up to the pattern then the supplied object
        strm << format_str.substr(current_pos, pos - current_pos) << obj;

        // move position passed the format
        pos += format_pattern.length();
    }
}

/**
 * Base case for variadic template unpacking.
 *
 * @param message
 *   Format string message.
 *
 * @param pos
 *   [in/out] On function enter this is the position to start searching in
 *   format_str for '{}'. On exit in the position in format_str after the '{}'.
 *   If no '{}' is found in the string then it is std::string::npos.
 *
 * @param strm
 *   The stream to write to.
 *
 * @param head
 *   The object to write.
 */
template <class Head>
void unpack(
    const std::string &message,
    std::size_t &pos,
    std::stringstream &strm,
    Head &&head)
{
    // write object to stream (if '{}' is in message)
    format(message, head, pos, strm);

    // we have no more args to write to stream, so write the remainder of the
    // message to the string (if there is any left)
    if (pos != std::string::npos)
    {
        strm << message.substr(pos);
    }
}

/**
 * Variadic templat argument unpacker.
 *
 * @param message
 *   Format string message.
 *
 * @param pos
 *   [in/out] On function enter this is the position to start searching in
 *   format_str for '{}'. On exit in the position in format_str after the '{}'.
 *   If no '{}' is found in the string then it is std::string::npos.
 *
 * @param strm
 *   The stream to write to.
 *
 * @param head
 *   Current object to write.
 *
 * @param args
 *   Remaining arguments
 */
template <class Head, class... Tail>
void unpack(
    const std::string &message,
    std::size_t &pos,
    std::stringstream &strm,
    Head &&head,
    Tail &&... tail)
{
    format(message, std::forward<Head>(head), pos, strm);
    unpack(message, pos, strm, std::forward<Tail>(tail)...);
}

}

/**
 * Singleton class for logging. Formatting and outputting are controlled via
 * settable classes, by default uses colour formatting and outputs to stdout.
 *
 * The supported log levels are:
 *   DEBUG,
 *   INFO,
 *   WARN,
 *   ERR
 *
 * It us up to you what to use each for however it is suggested that INFO be
 * used for the majority of logging, with WARN and ERR for warnings and errors
 * respectively (feel free to decide what constitutes as a warning and error).
 * Debug should be used for the log messages you use to diagnose a bug and will
 * probably later delete.
 */
class Logger
{
  public:
    /**
     * Get single instance of Logger.
     *
     * @returns
     *   Logger single instance.
     */
    static Logger &instance()
    {
        static Logger logger{};
        return logger;
    }

    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger &operator=(Logger &&) = delete;

    /**
     * Add a tag to be ignored, this prevents any log messages from the
     * given tag being processed.
     *
     * Adding the same tag more than once is a no-op.
     *
     * @param tag
     *   Tag to ignore.
     */
    void ignore_tag(const std::string &tag)
    {
        ignore_.emplace(tag);
    }

    /**
     * Show a supplied tag. This ensures that log messages from the given
     * tag are processed.
     *
     * Adding the same tag more than once or showing a non-hidden tag is a
     * no-op.
     *
     * @param tag
     *   Tag to show.
     */
    void show_tag(const std::string &tag)
    {
        if (const auto find = ignore_.find(tag); find != std::cend(ignore_))
        {
            ignore_.erase(find);
        }
    }

    /**
     * Set minimum log level, anything above this level is not processed.
     *
     * See log_level.h for definition of log_level.
     *
     * @param min_level
     *   Minimum level to process.
     */
    void set_min_level(const LogLevel min_level)
    {
        min_level_ = min_level;
    }

    /**
     * Set whether internal engine messages should be processed.
     *
     * @param log_engine
     *   True if engine messages should be processed, false of not.
     */
    void set_log_engine(const bool log_engine)
    {
        log_engine_ = log_engine;
    }

    /**
     * Set the Formatter class.
     *
     * Uses perfect forwarding to construct object.
     *
     * @param args
     *   Varaidic list of arguments for Formatter constructor.
     */
    template <
        class T,
        class... Args,
        typename = std::enable_if_t<std::is_base_of<Formatter, T>::value>>
    void set_Formatter(Args &&... args)
    {
        formatter_ = std::make_unique<T>(std::forward<Args>(args)...);
    }

    /**
     * Set the Outputter class.
     *
     * Uses perfect forwarding to construct object.
     *
     * @param args
     *   Varaidic list of arguments for Outputter constructor.
     */
    template <
        class T,
        class... Args,
        typename = std::enable_if_t<std::is_base_of<Outputter, T>::value>>
    void set_Outputter(Args &&... args)
    {
        outputter_ = std::make_unique<T>(std::forward<Args>(args)...);
    }

    /**
     * Log a message. This function handles the case where no arguments
     * are supplied i.e. just a log message.
     *
     * @param level
     *   Log level.
     *
     * @param tag
     *   Tag for log message.
     *
     * @param filename
     *   Name of the file logging the message.
     *
     * @param line
     *   Line of the log call in the file.
     *
     * @param engine
     *   True if this log message is from the internal engine, false
     *   otherwise.
     *
     * @param message
     *   Log message.
     */
    void log(
        const LogLevel level,
        const std::string &tag,
        const std::string &filename,
        const int line,
        const bool engine,
        const std::string &message)
    {
        // check if we want to process this log message
        if ((!engine || log_engine_) && (level >= min_level_) &&
            (ignore_.find(tag) == std::cend(ignore_)))
        {
            std::stringstream strm{};
            strm << message;

            const auto log =
                formatter_->format(level, tag, strm.str(), filename, line);

            std::unique_lock lock(mutex_);
            outputter_->output(log);
        }
    }

    /**
     * Log a message. This function handles the case where there are
     * arguments.
     *
     * @param level
     *   Log level.
     *
     * @param tag
     *   Tag for log message.
     *
     * @param filename
     *   Name of the file logging the message.
     *
     * @param line
     *   Line of the log call in the file.
     *
     * @param engine
     *   True if this log message is from the internal engine, false
     *   otherwise.
     *
     * @param message
     *   Log message.
     *
     * @param args
     *   Variadic list of arguments for log formatting.
     */
    template <class... Args>
    void log(
        const LogLevel level,
        const std::string &tag,
        const std::string &filename,
        const int line,
        const bool engine,
        const std::string &message,
        Args &&... args)
    {
        std::stringstream strm{};

        // apply string formatting
        std::size_t pos = 0u;
        detail::unpack(message, pos, strm, std::forward<Args>(args)...);

        log(level, tag, filename, line, engine, strm.str());
    }

  private:
    /**
     * Construct a new logger.
     */
    Logger()
        : formatter_(std::make_unique<ColourFormatter>())
        , outputter_(std::make_unique<StdoutFormatter>())
        , ignore_()
        , min_level_(LogLevel::DEBUG)
        , log_engine_(false)
        , mutex_(){};

    /** Formatter object. */
    std::unique_ptr<Formatter> formatter_;

    /** Outputter object. */
    std::unique_ptr<Outputter> outputter_;

    /** Collection of tags to ignore. */
    std::set<std::string> ignore_;

    /** Minimum log level. */
    LogLevel min_level_;

    /** Whether to log internal engine messages. */
    bool log_engine_;

    /** Lock for logging. */
    std::mutex mutex_;
};

}
