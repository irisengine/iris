////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/default_resource_manager.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>

#include "core/error_handling.h"

namespace iris
{

bool DefaultResourceManager::exists(std::string_view resource) const
{
    return std::filesystem::exists(root_ / resource);
}

DataBuffer DefaultResourceManager::do_load(std::string_view resource)
{
    std::stringstream strm{};
    std::fstream f(root_ / resource, std::ios::in | std::ios::binary);

    strm << f.rdbuf();

    ensure(f.good() && !f.bad(), "failed to read file");

    const auto str = strm.str();
    const auto *str_ptr = reinterpret_cast<const std::byte *>(str.data());

    return {str_ptr, str_ptr + str.length()};
}

void DefaultResourceManager::do_save(std::string_view resource, const DataBuffer &data)
{
    std::fstream f(root_ / resource, std::ios::out | std::ios::binary);
    f.write(reinterpret_cast<const char *>(data.data()), data.size());
}

}
