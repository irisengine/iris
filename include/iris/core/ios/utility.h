/**
 * This is an incomplete file and is intended to be included in
 * mac_ios_utility.mm
 *
 * *DO NOT* include this file directly, use macos_ios_utility.h
 */

#import <MetalKit/MetalKit.h>
#import <UIKit/UIKit.h>

#include "core/exception.h"
#include "core/ios/MetalView.h"

namespace
{

MetalView *get_view()
{
    const auto *window =
        [[[UIApplication sharedApplication] windows] objectAtIndex:0];
    if (window == nullptr)
    {
        throw iris::Exception("unable to get main window");
    }

    auto *view = static_cast<MetalView *>([[window rootViewController] view]);
    if (view == nullptr)
    {
        throw iris::Exception("unable to get metal view");
    }

    return view;
}

}

namespace iris::core::utility
{

id<MTLDevice> metal_device()
{
    const auto *view = get_view();

    auto *device = [view device];
    if (device == nullptr)
    {
        throw Exception("unable to get metal device from view");
    }

    return device;
}

CAMetalLayer *metal_layer()
{
    const auto *view = get_view();

    const auto *device = [view device];
    if (device == nullptr)
    {
        throw Exception("unable to get metal device from view");
    }

    auto *layer = [view metalLayer];
    if (layer == nullptr)
    {
        throw Exception("unable to get layer from view");
    }

    return (CAMetalLayer *)layer;
}

}
