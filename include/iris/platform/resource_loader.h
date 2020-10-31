#pragma once

#include <map>
#include <string>
#include <vector>

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
    const std::vector<std::uint8_t> &load(const std::string &resource);

    /**
     * Set root resource location.
     *
     * @param root
     *   New root location.
     */
    void set_root_directory(const std::string &root);

  private:
    /**
     * Construct a new ResourceLoader. Private to force instantiation
     * through instance.
     */
    ResourceLoader();

    /** Cache of loaded resources. */
    std::map<std::string, std::vector<std::uint8_t>> resources_;

    /** Resource root. */
    std::string root_;
};

}
