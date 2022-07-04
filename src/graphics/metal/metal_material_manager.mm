////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/metal/metal_material_manager.h"

#include "graphics/lights/light_type.h"
#include "graphics/metal/metal_mesh.h"
#include "graphics/render_entity.h"
#include "graphics/render_graph/render_graph.h"

namespace iris
{

Material *MetalMaterialManager::create(
    RenderGraph *render_graph,
    RenderEntity *render_entity,
    LightType light_type,
    bool,
    bool render_to_normal_target,
    bool render_to_position_target)
{
    return materials_.try_emplace(
        render_graph,
        light_type,
        render_to_normal_target,
        render_to_position_target,
        render_graph,
        static_cast<const MetalMesh *>(render_entity->mesh())->descriptors(),
        light_type,
        render_to_normal_target,
        render_to_position_target);
}

void MetalMaterialManager::clear()
{
    materials_.clear();
}

}
