#include "jobs/job_system_tests.h"

#include "jobs/fiber/fiber_job_system.h"

INSTANTIATE_TYPED_TEST_SUITE_P(fiber, JobSystemTests, iris::FiberJobSystem);
