#include "jobs/job.h"

#include <ostream>
#include <vector>

#include "jobs/job_system.h"

namespace
{
static iris::JobSystem js;
}

namespace iris::job
{

void add(const std::vector<Job> &jobs)
{
    js.add_jobs(jobs);
}

void wait(const std::vector<Job> &jobs)
{
    js.wait_for_jobs(jobs);
}

}