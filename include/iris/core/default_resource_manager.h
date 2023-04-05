////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string_view>

#include "core/data_buffer.h"
#include "core/resource_manager.h"

namespace iris
{

/**
 * Implementation of ResourceManager which loads resources off disk, relative to root.
 */
class DefaultResourceManager : public ResourceManager
{
  public:
    bool exists(std::string_view resource) const override;

  protected:
    /**
     * Load data frm disk.
     *
     * @param resource
     *   Name of resource.
     *
     * @returns
     *   Loaded data.
     */
    DataBuffer do_load(std::string_view resource) override;

    void do_save(std::string_view resource, const DataBuffer &data) override;
};

}
