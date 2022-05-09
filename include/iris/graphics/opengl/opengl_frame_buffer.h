////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/auto_release.h"
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_render_target.h"

namespace iris
{

/**
 * This is an internal class which encapsulates an OpenGL Frame Buffer Object (FBO). This is separate to the
 * OpenGLRenderTarget because, annoyingly, if we want multiple render targets we need to know all the outputs at the
 * time we create the FBO. This is cumbersome and makes the RenderTarget API hard to use. Instead we keep the
 * RenderTarget API simple for the user and internally in the OpenGLRenderer we create these OpenGLFrameBuffer objects
 * when we set a scene.
 */
class OpenGLFrameBuffer
{
  public:
    /**
     * Create a new empty OpenGLFrameBuffer.
     */
    OpenGLFrameBuffer() = default;

    /**
     * Create a new OpenGLFrameBuffer.
     *
     * @param colour_target
     *   Optional target to write colour data to.
     *
     * @param normal_target
     *   Optional target to write screen space normals to.
     *
     * @param position_target
     *   Optional target to write screen space positions to.
     */
    OpenGLFrameBuffer(
        const OpenGLRenderTarget *colour_target,
        const OpenGLRenderTarget *normal_target,
        const OpenGLRenderTarget *position_target);

    OpenGLFrameBuffer(const OpenGLFrameBuffer &) = delete;
    OpenGLFrameBuffer &operator=(const OpenGLFrameBuffer &) = delete;

    OpenGLFrameBuffer(OpenGLFrameBuffer &&) = default;
    OpenGLFrameBuffer &operator=(OpenGLFrameBuffer &&) = default;

    /**
     * Bind the FBO for rendering.
     */
    void bind() const;

    /**
     * Unbind the FBO.
     */
    void unbind() const;

    /**
     * Get the colour target.
     *
     * @return
     *   Colour target if set, otherwise nullptr.
     */
    const OpenGLRenderTarget *colour_target() const;

    /**
     * Get the normal target.
     *
     * @return
     *   Normal target if set, otherwise nullptr.
     */
    const OpenGLRenderTarget *normal_target() const;

    /**
     * Get the position target.
     *
     * @return
     *   Position target if set, otherwise nullptr.
     */
    const OpenGLRenderTarget *position_target() const;

  private:
    /** OpenGL handle for FBO. */
    AutoRelease<GLuint, 0u> handle_;

    /** Optional colour target. */
    const OpenGLRenderTarget *colour_target_;

    /** Optional normal target. */
    const OpenGLRenderTarget *normal_target_;

    /** Optional position target. */
    const OpenGLRenderTarget *position_target_;
};

}
