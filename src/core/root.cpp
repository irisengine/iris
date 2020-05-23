#include "core/root.h"

#include "jobs/job_system.h"
#include "log/logger.h"

namespace eng
{

Root Root::instance_;

Root::Root()
    : logger_(new Logger{ }),
      job_system_(new JobSystem{ })
{ }

Root::~Root()
{
    delete job_system_;

    // delete logger last as other destructors may use it
    delete logger_;
}

Root& Root::instance()
{
    return instance_;
}

JobSystem& Root::job_system()
{
    return *instance_.job_system_;
}

Logger& Root::logger()
{
    return *instance_.logger_;
}

}

