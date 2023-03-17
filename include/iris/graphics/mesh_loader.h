////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <functional>
#include <string_view>
#include <vector>

#include "core/resource_manager.h"
#include "graphics/animation/animation.h"
#include "graphics/skeleton.h"
#include "graphics/texture.h"
#include "graphics/vertex_data.h"
#include "graphics/weight.h"

namespace iris::mesh_loader
{

// callback aliases
using MeshDataCallback =
    std::function<void(std::vector<VertexData>, std::vector<std::uint32_t>, std::vector<Weight>, const std::string &)>;
using AnimationCallback = std::function<void(std::vector<Animation>, Skeleton)>;

/**
 * Load a mesh from file.
 *
 * @param resource_manager
 *   Resource manager object.
 *
 * @param mesh_name
 *   Name of of mesh to load, will be passed to ResourceLoader.
 *
 * @param flip_uvs
 *   True if uvs should be flipped, false otherwise.
 *
 * @param mesh_data_callback
 *   Callback for returning loaded mesh data.
 *
 * @param animation_callback
 *   Callback for returning loaded animation data.
 */
void load(
    ResourceManager &resource_manager,
    std::string_view mesh_name,
    bool flip_uvs,
    MeshDataCallback mesh_data_callback,
    AnimationCallback animation_callback);

}
