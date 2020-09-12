#include "platform/start.h"

#include <functional>

#import <UIKit/UIKit.h>

#import "platform/ios/AppDelegate.h"
#include "core/root.h"
#include "log/emoji_formatter.h"
#include "log/log.h"

namespace iris
{

// globals so we can call back into game
// nasty but effective
std::function<void(int, char**)> g_entry;
int g_argc;
char **g_argv;

void start(int argc, char **argv, std::function<void(int, char**)> entry)
{
    // xcode doesn't support ANSI colour codes so we default to the emoji
    // formatter
    Root::logger().set_Formatter<EmojiFormatter>();

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
        ::UIApplicationMain(
            argc,
            argv,
            nil,
            ::NSStringFromClass([AppDelegate class]));
    }
}

void start_debug(int argc, char **argv, std::function<void(int, char**)> entry)
{
    // enable engine logging
    Root::logger().set_Formatter<EmojiFormatter>();
    Root::logger().set_log_engine(true);

    LOG_ENGINE_INFO("start", "engine start (with debugging)");

    start(argc, argv, entry);
}

}

