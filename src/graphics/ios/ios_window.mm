////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/ios/ios_window.h"

#include <cstdint>
#include <memory>

#import <UIKit/UIKit.h>

#import "graphics/ios/metal_view_controller.h"
#include "graphics/metal/metal_renderer.h"
#include "log/log.h"

namespace iris
{

IOSWindow::IOSWindow(std::uint32_t width, std::uint32_t height)
    : Window(width, height)
{
    const auto bounds = [[UIScreen mainScreen] bounds];
    width_ = bounds.size.width;
    height_ = bounds.size.height;

    renderer_ = std::make_unique<MetalRenderer>(width_, height_);
}

std::optional<Event> IOSWindow::pump_event()
{
    const CFTimeInterval seconds = 0.000002;

    // run the default loop, this pumps touch events which will then be picked
    // up by our view
    auto result = kCFRunLoopRunHandledSource;
    do
    {
        result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, seconds, TRUE);
    } while (result == kCFRunLoopRunHandledSource);

    const auto *window = [[[UIApplication sharedApplication] windows] objectAtIndex:0];
    const auto *root_view_controller = static_cast<MetalViewController *>([window rootViewController]);

    std::optional<Event> event;

    // get next event from view (if one is available)
    if (!root_view_controller->events_.empty())
    {
        event = root_view_controller->events_.front();
        root_view_controller->events_.pop();
    }

    return event;
}

std::uint32_t IOSWindow::screen_scale() const
{
    static std::uint32_t scale = 0u;

    if (scale == 0u)
    {
        scale = static_cast<std::uint32_t>([[UIScreen mainScreen] scale]);
    }

    return scale;
}

}
