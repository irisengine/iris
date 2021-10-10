////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/resource_loader.h"

#include <fstream>
#include <sstream>

#import <UIKit/UIKit.h>

#include "core/exception.h"
#include "core/macos/macos_ios_utility.h"

namespace iris
{

ResourceLoader::ResourceLoader(){};

ResourceLoader &ResourceLoader::instance()
{
    static ResourceLoader loader{};
    return loader;
}

const DataBuffer &ResourceLoader::load(const std::string &resource)
{
    // lookup resource
    auto loaded_resource = resources_.find(resource);

    // if not found load from disk, treat resource as a path relative to
    // the app bundle resource path
    if (loaded_resource == std::cend(resources_))
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

        const auto [iter, _] = resources_.insert({resource, {std::cbegin(str), std::cend(str)}});
        loaded_resource = iter;
    }

    return loaded_resource->second;
}

}
