////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "core/data_buffer.h"
#include "core/string_hash.h"

namespace iris
{

/**
 * An abstract class for loading and caching resources.
 */
class ResourceManager
{
  public:
    ResourceManager();

    virtual ~ResourceManager() = default;

    /**
     * Load a resource. If this is the first load of resource then it is fetched (via the deriving classes
     * implementation). Otherwise a cached copy is returned.
     *
     * @param resource
     *   Name of resource.
     *
     * @returns
     *   Const reference to loaded data.
     */
    const DataBuffer &load(std::string_view resource);

    /**
     * Set root resource location. Note that implementations may choose to ignore this.
     *
     * @param root
     *   New root location.
     */
    void set_root_directory(const std::filesystem::path &root);

    std::vector<std::string> available_resources() const;

  protected:
    /**
     * Implementations should override this to perform their specific data loading logic.
     *
     * @param resource
     *   Name of resource.
     *
     * @returns
     *   Loaded data.
     */
    virtual DataBuffer do_load(std::string_view resource) = 0;

    /** Resource root. */
    std::filesystem::path root_;

  private:
    /** Cache of loaded resources. */
    std::unordered_map<std::string, DataBuffer, StringHash, std::equal_to<>> resources_;
};

}
