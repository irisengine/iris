////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>

namespace iris
{

/**
 * Start the engine! This performs all platform and subsystem initialisation
 * before calling the supplied entry.
 *
 * @param argc
 *   argc from main()
 *
 * @param argv
 *   argv from main()
 *
 * @param entry
 *   Entry point into game, will be passed argc and argv back.
 */
void start(int argc, char **argv, std::function<void(int, char **)> entry);

/**
 * Start the engine! This performs all platform and subsystem initialisation
 * before calling the supplied entry. This enables additional debugging and is
 * mainly used for diagnosing engine issues.
 *
 * @param argc
 *   argc from main()
 *
 * @param argv
 *   argv from main()
 *
 * @param entry
 *   Entry point into game, will be passed argc and argv back.
 */
void start_debug(
    int argc,
    char **argv,
    std::function<void(int, char **)> entry);

}
