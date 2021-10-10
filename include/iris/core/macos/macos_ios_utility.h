////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#import <MetalKit/MetalKit.h>

namespace iris::core::utility
{

/**
 * Helper function to convert a std::string to a NSString.
 *
 * @param str
 *   String to convert.
 *
 * @returns
 *   Supplied string converted to NSString.
 */
NSString *string_to_nsstring(const std::string &str);

/**
 * Get the metal device object for the current view.
 *
 * @returns
 *   Metal device.
 */
id<MTLDevice> metal_device();

/**
 * Get the metal layer for the current view.
 *
 * @returns
 *   Metal layer.
 */
CAMetalLayer *metal_layer();

}
