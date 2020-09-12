#include "platform/start.h"

#include <iostream>
#include <functional>

#include "core/root.h"
#include "jobs/job_system.h"
#include "log/log.h"
#include "log/logger.h"

namespace iris
{

void start(int argc, char **argv, std::function<void(int, char**)> entry)
{
    // safe to initialise root now
    Root::instance().init();

    LOG_ENGINE_INFO("start", "engine start");

    entry(argc, argv);
}

void start_debug(int argc, char **argv, std::function<void(int, char**)> entry)
{
    // safe to initialise root now
    Root::instance().init();

    // enable engine logging
    Root::logger().set_log_engine(true);

    Root::job_system().set_stats_stream(&std::cout);

    LOG_ENGINE_INFO("start", "engine start (with debugging)");

    entry(argc, argv);
}

}

