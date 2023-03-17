////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <gmock/gmock.h>

#include "core/resource_manager.h"
#include "graphics/mesh.h"
#include "graphics/mesh_manager.h"
#include "graphics/vertex_data.h"

class MockMeshManager : public iris::MeshManager
{
  public:
    MockMeshManager(iris::ResourceManager &resource_manager)
        : MeshManager(resource_manager, true)
    {
    }

  protected:
    MOCK_METHOD(
        std::unique_ptr<iris::Mesh>,
        create_mesh,
        (const std::vector<iris::VertexData> &, const std::vector<std::uint32_t> &),
        (const, override));
};
