////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graphics/render_target.h"
#include "graphics/texture.h"

namespace iris
{

/**
 * Implementation of RenderTarget for OpenGL.
 */
class OpenGLRenderTarget : public RenderTarget
{
  public:
    /**
     * Construct a new OpenGLRenderTarget.
     *
     * @param colour_texture
     *   Texture to render colour data to.
     *
     * @param depth_texture
     *   Texture to render depth data to.
     */
    OpenGLRenderTarget(const Texture *colour_texture, const Texture *depth_texture);
};

}
