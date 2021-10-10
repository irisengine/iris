////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "core/data_buffer.h"

namespace iris
{

/**
 * Singleton class for loading and caching resources.
 */
class ResourceLoader
{
  public:
    /**
     * Get single instance of class.
     *
     * @returns
     *   Reference to single instance.
     */
    static ResourceLoader &instance();

    /**
     * Load a resource. If this is the first load of resource then it is
     * fetched from the platform specific backing store (e.g. disk).
     * Otherwise a cached copy is returned.
     *
     * @param resource
     *   Name of resource. This should be a '/' separated path relative
     *   to the root resource location.
     *
     * @returns
     *   Const reference to loaded data.
     */
    const DataBuffer &load(const std::string &resource);

    /**
     * Set root resource location.
     *
     * @param root
     *   New root location.
     */
    void set_root_directory(const std::filesystem::path &root);

  private:
    /**
     * Construct a new ResourceLoader. Private to force instantiation
     * through instance.
     */
    ResourceLoader();

    /** Cache of loaded resources. */
    std::map<std::string, DataBuffer> resources_;

    /** Resource root. */
    std::filesystem::path root_;
};

}
