#pragma once

#include <cstdint>
#include <string>
#include <vector>

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
