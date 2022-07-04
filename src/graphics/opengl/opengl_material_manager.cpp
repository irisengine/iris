////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/opengl_material_manager.h"

#include "graphics/material_cache.h"
#include "graphics/opengl/opengl_render_target.h"
#include "graphics/render_entity.h"
#include "graphics/render_graph/render_graph.h"

namespace iris
{

Material *OpenGLMaterialManager::create(
    RenderGraph *render_graph,
    RenderEntity *,
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
        light_type,
        render_to_normal_target,
        render_to_position_target);
}

void OpenGLMaterialManager::clear()
{
    materials_.clear();
}

}
