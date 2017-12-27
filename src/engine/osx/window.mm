#include "window.hpp"

#include <stdexcept>

#import <Appkit/Appkit.h>
#import <Foundation/Foundation.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#include "osx/AppDelegate.h"

namespace eng
{

window::window(const float width, const float height)
    : width_(width),
      height_(height)
{
    // get and/or create the application singleton
    NSApplication *app = [NSApplication sharedApplication];

    // this is an ordinary app
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    // make this app the active app
    [NSApp activateIgnoringOtherApps:YES];

    // create our app delegate, this will handle the window creation
    AppDelegate *appDelegate = [[AppDelegate alloc] initWithRect:NSMakeRect(0.0f, 0.0f, width_, height_)];

    // check that we created the delegate
    if(appDelegate == nil)
    {
        throw std::runtime_error("failed to create AppDelegate");
    }

    // set the delegate
    [app setDelegate:appDelegate];

    // activate the app
    [app finishLaunching];

    // create an auto release pool for this thread, according to the Apple
    // docs this will get automatically drained when the main thread
    // terminates
    // http://tinyurl.com/nmdem83
    [[NSAutoreleasePool alloc] init];

    [NSCursor hide];

    // perform OpenGl setup
    ::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void window::pre_render() const noexcept
{
    NSEvent *event = nil;

    do
    {
        // flush next event
        event = [NSApp
            nextEventMatchingMask:NSEventMaskAny
            untilDate:[NSDate distantPast]
            inMode:NSDefaultRunLoopMode
            dequeue:YES];

        // dispatch the event to other objects, this stops us swallowing
        // all events and preventing anything else from receiving them
        [NSApp sendEvent:event];
    } while(event != nil);

    // clear current target
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void window::post_render() const noexcept
{
    ::glSwapAPPLE();
}

float window::width() const noexcept
{
    return width_;
}

float window::height() const noexcept
{
    return height_;
}

}

