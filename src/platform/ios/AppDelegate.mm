#import "platform/ios/AppDelegate.h"

#include <functional>

#import <CoreGraphics/CoreGraphics.h>
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>

#include "log/log.h"
#include "platform/ios/MetalView.h"
#include "platform/ios/MetalViewController.h"

namespace eng
{

// globals for calling back into game
extern std::function<void(int, char**)> g_entry;
extern int g_argc;
extern char **g_argv;

}

@interface AppDelegate ()

@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    LOG_ENGINE_INFO("AppDelegate", "setting up window and view");

    // create a new window the size of the screen
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

    // create metal view and view controller
    auto *view_controller = [[MetalViewController alloc] init];
    auto *view = [[MetalView alloc] init];
    [view_controller setView:view];

    // make window visible
    [[self window] setRootViewController:view_controller];
    [[self window] makeKeyAndVisible];
    [[self window] setNeedsDisplay];

    // fire of a call so the game entry point gets called
    // this has to be done via a selector as didFinishLaunchingWithOptions
    // must return, or else nothing will get rendered
    [self performSelector:@selector(callEntry) withObject:nil afterDelay:0.0];

    LOG_ENGINE_INFO("AppDelegate", "launch done");

    return YES;
}

- (void)callEntry {
    LOG_ENGINE_INFO("AppDelegate", "calling main");

    eng::g_entry(eng::g_argc, eng::g_argv);

    LOG_ENGINE_INFO("AppDelegate", "main done");
}

@end

