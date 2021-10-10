////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

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
