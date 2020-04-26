#include "platform/resource_loader.h"

#include <fstream>
#include <sstream>

namespace eng
{

ResourceLoader::ResourceLoader()
{
};

ResourceLoader& ResourceLoader::instance()
{
    static ResourceLoader loader{ };
    return loader;
}

const std::vector<std::uint8_t>& ResourceLoader::load(const std::string &resource)
{
    // lookup resource
    auto loaded_resource = resources_.find(resource);
    
    // if not found load from disk, treat resource as a path relative to
    // current working directory
    if(loaded_resource == std::cend(resources_))
    {
        std::stringstream strm{ };
        std::fstream f(resource, std::ios::in | std::ios::binary);
        strm << f.rdbuf();

        const auto str = strm.str();

        const auto [iter, _] = resources_.insert({
            resource,
            { std::cbegin(str), std::cend(str) }
        });
        loaded_resource = iter;
    }

    return loaded_resource->second;
}

}

