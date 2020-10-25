#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "graphics/skeleton.h"
#include "graphics/texture.h"
#include "graphics/vertex_data.h"

namespace iris
{

/**
 * Load a mesh from file and return requests parts.
 *
 * Note that this function does not create a Mesh object as we don't know how
 * the user wishes to construct their vertex buffers. Instead we return vertex
 * data in the engine default struct, which supports everything. It is then up
 * to the user to transform this data into their required Buffer objects (or
 * just use the returned data).
 *
 * @param mesh_name
 *   Name of of mesh to load, will be passed to ResourceLoader.
 *
 * @param vertices
 *   Optional pointer to collection of vertex data to fill.
 *
 * @param indices
 *   Optional pointer to collection of indices to fill.
 *
 * @param textures
 *   Optional pointer to collection of textures to fill.
 *
 * @param skeleton
 *   Optional pointer to skeleton to create
 */
void load_mesh(
    const std::string &mesh_name,
    std::vector<std::vector<vertex_data>> *vertices,
    std::vector<std::vector<std::uint32_t>> *indices,
    std::vector<Texture *> *textures,
    Skeleton *skeleton);

}
