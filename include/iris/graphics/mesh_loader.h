////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "graphics/animation/animation.h"
#include "graphics/skeleton.h"
#include "graphics/texture.h"
#include "graphics/vertex_data.h"

namespace iris::mesh_loader
{

/**
 * Helper struct encapsulating loaded mesh data.
 */
struct LoadedData
{
    /** Mesh vertices. */
    std::vector<VertexData> vertices;

    /** Mesh indices. */
    std::vector<std::uint32_t> indices;

    /** Mesh skeleton. */
    Skeleton skeleton;

    /** Mesh animations. */
    std::vector<Animation> animations;
};

/**
 * Load a mesh from file and return its data.c
 *
 * @param mesh_name
 *   Name of of mesh to load, will be passed to ResourceLoader.
 *
 * @returns
 *   Data loaded fro file.
 */
LoadedData load(const std::string &mesh_name);

}
