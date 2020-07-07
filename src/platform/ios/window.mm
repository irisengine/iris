#include "platform/window.h"

#include <memory>

#import <UIKit/UIKit.h>

#import "platform/ios/MetalViewController.h"
#include "core/real.h"
#include "graphics/font.h"
#include "log/log.h"

namespace eng
{

Window::Window(real width, real height)
    : width_(width),
      height_(height)
{
    const auto bounds = [[UIScreen mainScreen] bounds];
    width_ = bounds.size.width;
    height_ = bounds.size.height;
}

std::optional<Event> Window::pump_event()
{
    const CFTimeInterval seconds = 0.000002;

    // run the default loop, this pumps touch events which will then be picked
    // up by our view
    auto result = kCFRunLoopRunHandledSource;
    do
    {
        result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, seconds, TRUE);
    } while(result == kCFRunLoopRunHandledSource);
    
    const auto *window = [[[UIApplication sharedApplication] windows] objectAtIndex:0];
    const auto *root_view_controller = static_cast<MetalViewController*>([window rootViewController]);
    
    std::optional<Event> event;
    
    // get next event from view (if one is available)
    if(!root_view_controller->events_.empty())
    {
        event = root_view_controller->events_.front();
        root_view_controller->events_.pop();
    }
    
    return event;
}

real Window::width() const
{
    return width_;
}

real Window::height() const
{
    return height_;
}

}
