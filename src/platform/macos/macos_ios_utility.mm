#include "platform/macos/macos_ios_utility.h"

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

namespace iris::platform::utility
{

// common utilities

NSString* string_to_nsstring(const std::string &str)
{
    return [NSString stringWithUTF8String:str.c_str()];
}

}

// platform specific implementations

#if defined(PLATFORM_MACOS)
#include "platform/macos/utility.h"
#elif defined(PLATFORM_IOS)
#include "platform/ios/utility.h"
#endif
