////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

/**
 * This is an incomplete file and is intended to be included in
 * mac_ios_utility.mm
 *
 * *DO NOT* include this file directly, use macos_ios_utility.h
 */

#import <Metal/Metal.h>

#include "core/exception.h"

namespace iris::core::utility
{

id<MTLDevice> metal_device()
{
    return ::CGDirectDisplayCopyCurrentMetalDevice(::CGMainDisplayID());
}

CAMetalLayer *metal_layer()
{
    // get a pointer to the main window
    auto *window = [[NSApp windows] firstObject];
    if (window == nullptr)
    {
        throw Exception("could not get main window");
    }

    // get a pointer to the metal layer to render to
    auto *layer = static_cast<CAMetalLayer *>([[window contentView] layer]);
    if (layer == nullptr)
    {
        throw Exception("could not get metal later");
    }

    return layer;
}

}
