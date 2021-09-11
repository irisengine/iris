#pragma once

#include <memory>

#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_texture.h"
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
    OpenGLRenderTarget(
        std::unique_ptr<Texture> colour_texture,
        std::unique_ptr<Texture> depth_texture);

    /**
     * Clean up OpenGL objects.
     */
    ~OpenGLRenderTarget() override;

    /**
     * Bind render target for use.
     *
     * @param target
     *   OpenGL framebuffer target.
     */
    void bind(GLenum target) const;

    /**
     * Unbind the render target for use.
     *
     * @param target
     *   OpenGL framebuffer target.
     */
    void unbind(GLenum target) const;

    void multisample_resolve() const;

  private:
    /** OpenGL handle to framebuffer */
    GLuint handle_;

    GLuint multisample_handle_;

    std::unique_ptr<OpenGLTexture> multisample_colour_texture_;
    std::unique_ptr<OpenGLTexture> multisample_depth_texture_;
};

}
