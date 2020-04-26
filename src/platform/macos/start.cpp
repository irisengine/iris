#include "platform/start.h"

#include <functional>

#include "log/log.h"
#include "log/logger.h"

namespace eng
{

void start(int argc, char **argv, std::function<void(int, char**)> entry)
{
    LOG_ENGINE_INFO("start", "engine start");

    entry(argc, argv);
}

void start_debug(int argc, char **argv, std::function<void(int, char**)> entry)
{
    // enable engine logging
    Logger::instance().set_log_engine(true);

    LOG_ENGINE_INFO("start", "engine start (with debugging)");

    entry(argc, argv);
}

}

