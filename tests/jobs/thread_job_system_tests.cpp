#include "jobs/job_system_tests.h"

#include "jobs/thread/thread_job_system.h"

INSTANTIATE_TYPED_TEST_SUITE_P(thread, JobSystemTests, iris::ThreadJobSystem);
