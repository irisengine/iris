////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/start.h"

#include <functional>

#import <UIKit/UIKit.h>

#import "graphics/ios/app_delegate.h"

#include "core/root.h"
#include "graphics/ios/ios_window_manager.h"
#include "graphics/metal/metal_mesh_manager.h"
#include "graphics/metal/metal_texture_manager.h"
#include "iris_version.h"
#include "jobs/thread/thread_job_system_manager.h"
#include "log/emoji_formatter.h"
#include "log/log.h"
#include "log/logger.h"
#include "physics/bullet/bullet_physics_manager.h"

namespace
{

/**
 * Register apis and set default.s
 */
void register_apis()
{
    iris::Root::register_graphics_api(
        "metal",
        std::make_unique<iris::IOSWindowManager>(),
        std::make_unique<iris::MetalMeshManager>(),
        std::make_unique<iris::MetalTextureManager>());
    iris::Root::set_graphics_api("metal");

    iris::Root::register_physics_api("bullet", std::make_unique<iris::BulletPhysicsManager>());
    iris::Root::set_physics_api("bullet");

    iris::Root::register_jobs_api("thread", std::make_unique<iris::ThreadJobSystemManager>());
    iris::Root::set_jobs_api("thread");
}

}

namespace iris
{

// globals so we can call back into game
// nasty but effective
std::function<void(int, char **)> g_entry;
int g_argc;
char **g_argv;

void start(int argc, char **argv, std::function<void(int, char **)> entry)
{
    // xcode doesn't support ANSI colour codes so we default to the emoji
    // formatter
    Logger::instance().set_Formatter<EmojiFormatter>();

    register_apis();

    LOG_ENGINE_INFO("start", "engine start");

    // save off supplied variables for use later
    g_entry = entry;
    g_argc = argc;
    g_argv = argv;

    @autoreleasepool
    {
        // start the main ios application
        // this is why we have to store the function arguments as globals as
        // we have no way of accessing them in the AppDelegate
        ::UIApplicationMain(argc, argv, nil, ::NSStringFromClass([AppDelegate class]));

        Root::reset();
    }
}

void start_debug(int argc, char **argv, std::function<void(int, char **)> entry)
{
    // enable engine logging
    Logger::instance().set_Formatter<EmojiFormatter>();
    Logger::instance().set_log_engine(true);

    LOG_ENGINE_INFO("start", "engine start (with debugging)");

    register_apis();

    start(argc, argv, entry);
}

}
