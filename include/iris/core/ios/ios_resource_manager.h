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
 * Implementation of ResourceManager which loads files on iOS.
 */
class IOSResourceManager : public ResourceManager
{
  protected:
    /**
     * Load files from disk o iOS.
     *
     * @param resource
     *   Name of resource.
     *
     * @returns
     *   Loaded data.
     */
    DataBuffer do_load(std::string_view resource) override;
};

}
