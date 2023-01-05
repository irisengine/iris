////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_material_manager.h"

#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_mesh.h"
#include "graphics/d3d12/d3d12_render_target.h"
#include "graphics/material_cache.h"
#include "graphics/render_entity.h"
#include "graphics/render_graph/render_graph.h"

namespace iris
{

Material *D3D12MaterialManager::create(
    RenderGraph *render_graph,
    RenderEntity *render_entity,
    LightType light_type,
    bool render_to_colour_target,
    bool render_to_normal_target,
    bool render_to_position_target,
    bool has_transparency)
{
    return materials_.try_emplace(
        render_graph,
        light_type,
        render_to_normal_target,
        render_to_position_target,
        render_graph,
        static_cast<const D3D12Mesh *>(render_entity->mesh())->input_descriptors(),
        render_entity->primitive_type(),
        light_type,
        D3D12Context::root_signature().handle(),
        !render_to_colour_target,
        render_to_normal_target,
        render_to_position_target,
        has_transparency,
        render_entity->should_render_wireframe());
}

void D3D12MaterialManager::clear()
{
    materials_.clear();
}

}
