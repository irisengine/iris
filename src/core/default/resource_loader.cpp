#include "core/resource_loader.h"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "core/error_handling.h"

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

const DataBuffer &ResourceLoader::load(const std::string &resource)
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

        ensure(f.good() && !f.bad(), "failed to read file");

        const auto str = strm.str();
        const auto *str_ptr = reinterpret_cast<const std::byte *>(str.data());

        const auto [iter, _] =
            resources_.insert({resource, {str_ptr, str_ptr + str.length()}});
        loaded_resource = iter;
    }

    return loaded_resource->second;
}

void ResourceLoader::set_root_directory(const std::filesystem::path &root)
{
    root_ = root;
}
}
