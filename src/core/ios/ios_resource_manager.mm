////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/ios_resource_manager.h"

#include <fstream>
#include <sstream>
#include <string_view>

#import <UIKit/UIKit.h>

#include "core/exception.h"
#include "core/macos/macos_ios_utility.h"

namespace iris
{

DataBuffer ResourceManager::load(std::string_view resource)
{
    const auto *bundle = [NSBundle mainBundle];
    if (bundle == nullptr)
    {
        throw Exception("could not get main bundle");
    }

    const auto *dir = [bundle resourcePath];
    if (dir == nullptr)
    {
        throw Exception("could not resolve path to resouce path");
    }

    const auto *resource_ns = core::utility::string_to_nsstring(resource);
    auto *parts = [NSArray arrayWithObjects:dir, resource_ns, (void *)nil];
    const auto *path = [NSString pathWithComponents:parts];
    const auto *cpath = [path fileSystemRepresentation];

    std::stringstream strm{};
    std::fstream f(cpath, std::ios::in | std::ios::binary);
    strm << f.rdbuf();

    const auto str = strm.str();
    const auto *ptr = reinterpret_cast<const std::byte *>(str.data());

    return {ptr, ptr + str.size()};
}

}
