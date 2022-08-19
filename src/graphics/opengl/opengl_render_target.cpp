////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/opengl_render_target.h"

#include "graphics/texture.h"

namespace iris
{

OpenGLRenderTarget::OpenGLRenderTarget(const Texture *colour_texture, const Texture *depth_texture)
    : RenderTarget(colour_texture, depth_texture)
{
}

}
