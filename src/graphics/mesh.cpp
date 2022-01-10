////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/mesh.h"

#include <cstdint>
#include <functional>
#include <vector>

#include "graphics/vertex_data.h"

namespace iris
{

Mesh::Mesh(const std::vector<VertexData> &vertices, const std::vector<std::uint32_t> &indices)
    : vertices_(vertices)
    , indices_(indices)
{
}

const std::vector<VertexData> &Mesh::vertices() const
{
    return vertices_;
}

const std::vector<std::uint32_t> &Mesh::indices() const
{
    return indices_;
}

}
