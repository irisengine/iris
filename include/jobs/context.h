#pragma once

namespace iris
{

// include arch specific files

#if defined(IRIS_ARCH_X86_64)
#include "jobs/arch/x86_64/context.h"
#else
#error unsupported architecture
#endif

}

