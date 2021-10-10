////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/macos/macos_ios_utility.h"

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

namespace iris::core::utility
{

// common utilities

NSString* string_to_nsstring(const std::string &str)
{
    return [NSString stringWithUTF8String:str.c_str()];
}

}

// platform specific implementations

#if defined(IRIS_PLATFORM_MACOS)
#include "core/macos/utility.h"
#elif defined(IRIS_PLATFORM_IOS)
#include "core/ios/utility.h"
#endif
