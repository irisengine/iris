////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <optional>
#include <string_view>

#include "core/transform.h"

namespace iris
{

/**
 * Interface for querying bone transformations.
 */
class BoneQuery
{
  public:
    virtual ~BoneQuery() = default;

    /**
     * Get the transformation for a bone.
     *
     * @param bone_name
     *   Name of the bone to query.
     *
     * @returns
     *   Bone transform, if bone exists.
     */
    virtual std::optional<Transform> transform(std::string_view bone_name) = 0;
};

}
