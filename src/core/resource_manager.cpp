////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/resource_manager.h"

#include <filesystem>
#include <fstream>
#include <ranges>
#include <sstream>
#include <vector>

#include "core/error_handling.h"

namespace iris
{

ResourceManager::ResourceManager()
    : root_(".")
{
}

const DataBuffer &ResourceManager::load(std::string_view resource)
{
    // lookup resource
    auto loaded_resource = resources_.find(resource);

    // if not found load from disk, treat resource as a path relative to
    // root
    if (loaded_resource == std::cend(resources_))
    {
        const auto [iter, _] = resources_.insert({std::string{resource}, do_load(resource)});
        loaded_resource = iter;
    }

    return loaded_resource->second;
}

void ResourceManager::set_root_directory(const std::filesystem::path &root)
{
    root_ = root;
}

std::vector<std::string> ResourceManager::available_resources() const
{
    std::vector<std::string> paths{};
    const auto iter = std::filesystem::directory_iterator{root_};

    std::transform(
        std::filesystem::begin(iter),
        std::filesystem::end(iter),
        std::back_inserter(paths),
        [](const auto &path) { return path.path().filename().native(); });

    std::ranges::sort(paths);

    return paths;
}

}
