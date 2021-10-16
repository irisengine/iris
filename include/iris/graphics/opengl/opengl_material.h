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
#include "graphics/texture.h"

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
     */
    OpenGLMaterial(const RenderGraph *render_graph, LightType light_type);

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

    /**
     * Get all textures used by this material.
     *
     * @returns
     *   Collection of Texture objects used by this material.
     */
    std::vector<Texture *> textures() const override;

  private:
    /** OpenGL handle to material. */
    GLuint handle_;

    /** Collection of Texture objects used by material. */
    std::vector<Texture *> textures_;
};

}
