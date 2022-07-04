////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graphics/lights/light_type.h"
#include "graphics/material_cache.h"
#include "graphics/material_manager.h"
#include "graphics/opengl/opengl_material.h"

namespace iris
{

class Material;
class RenderGraph;
class RenderTarget;

class OpenGLMaterialManager : public MaterialManager
{
  public:
    ~OpenGLMaterialManager() override = default;

    Material *create(
        RenderGraph *render_graph,
        RenderEntity *render_entity,
        LightType light_type,
        bool render_to_colour_target,
        bool render_to_normal_target,
        bool render_to_position_target);

    void clear() override;

  private:
    MaterialCache<OpenGLMaterial, RenderGraph *, LightType, bool, bool> materials_;
};

}
