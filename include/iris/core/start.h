////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>

#include "core/context.h"

namespace iris
{

/**
 * Start the engine! This performs all platform and subsystem initialisation before calling the supplied entry.
 *
 * @param argc
 *   argc from main()
 *
 * @param argv
 *   argv from main()
 *
 * @param entry
 *   Entry point into game, will be passed an engine context object.
 *
 * @param debug
 *   If the engine should be started in debug mode.
 */
void start(int argc, char **argv, std::function<void(Context)> entry, bool debug = false);

}
