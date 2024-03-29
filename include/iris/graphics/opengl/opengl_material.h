////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#include "graphics/lights/light_type.h"
#include "graphics/material.h"
#include "graphics/opengl/opengl.h"
#include "graphics/render_graph/render_graph.h"

namespace iris
{

/**
 * Implementation of Material for OpenGL.
 */
class OpenGLMaterial : public Material
{
  public:
    /**
     * Construct a new OpenGLMaterial.
     *
     * @param render_graph
     *   RenderGraph that describes the material.
     *
     * @param light_type
     *   Type of light for this material.
     *
     * @param render_to_normal_target
     *   Flag indicating whether the material should also write out screen space normals to a render texture.
     *
     * @param render_to_position_target
     *   Flag indicating whether the material should also write out screen space positions to a render texture.
     */
    OpenGLMaterial(
        const RenderGraph *render_graph,
        LightType light_type,
        bool render_to_normal_target,
        bool render_to_position_target);

    /**
     * Clean up OpenGL objects.
     */
    ~OpenGLMaterial() override;

    /**
     * Bind this material for rendering with.
     */
    void bind() const;

    /**
     * Get the OpenGL handle to this material.
     *
     * @returns
     *   OpenGL handle.
     */
    GLuint handle() const;

  private:
    /** OpenGL handle to material. */
    GLuint handle_;
};

}
