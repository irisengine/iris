#pragma once

#include <functional>
#include <ostream>
#include <vector>

namespace iris
{

// convenient alias for a job
using Job = std::function<void()>;

namespace job
{

/**
 * Add a collection of jobs. Once added these are executed in a
 * fire-and-forget manner, there is no way to wait on them to finish or
 * to know when they have executed.
 *
 * @param jobs
 *   Jobs to execute.
 */
void add(const std::vector<Job> &jobs);

/**
 * Add a collection of jobs. Once added this call blocks until all
 * jobs have finished executing.
 *
 * @param jobs
 *   Jobs to execute.
 */
void wait(const std::vector<Job> &jobs);

}

}
