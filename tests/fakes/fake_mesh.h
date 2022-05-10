////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <vector>

#include "graphics/mesh.h"
#include "graphics/vertex_data.h"

class FakeMesh : public iris::Mesh
{
  public:
    FakeMesh()
        : Mesh({}, {})
    {
    }

    ~FakeMesh() override = default;

    void update_vertex_data(const std::vector<iris::VertexData> &data) override
    {
    }

    void update_index_data(const std::vector<std::uint32_t> &data) override
    {
    }
};
