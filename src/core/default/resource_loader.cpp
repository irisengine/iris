#include "core/resource_loader.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace iris
{

ResourceLoader::ResourceLoader()
    : root_(".")
{
}

ResourceLoader &ResourceLoader::instance()
{
    static ResourceLoader loader{};
    return loader;
}

const std::vector<std::uint8_t> &ResourceLoader::load(
    const std::string &resource)
{
    // lookup resource
    auto loaded_resource = resources_.find(resource);

    // if not found load from disk, treat resource as a path relative to
    // root
    if (loaded_resource == std::cend(resources_))
    {
        std::stringstream strm{};
        std::fstream f(
            root_ / std::filesystem::path(resource),
            std::ios::in | std::ios::binary);
        strm << f.rdbuf();

        const auto str = strm.str();

        const auto [iter, _] =
            resources_.insert({resource, {std::cbegin(str), std::cend(str)}});
        loaded_resource = iter;
    }

    return loaded_resource->second;
}

void ResourceLoader::set_root_directory(const std::filesystem::path &root)
{
    root_ = root;
}

}
