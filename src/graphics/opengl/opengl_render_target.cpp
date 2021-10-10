////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/opengl_render_target.h"

#include <cstdint>

#include "core/error_handling.h"
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_texture.h"

namespace iris
{

OpenGLRenderTarget::OpenGLRenderTarget(
    std::unique_ptr<Texture> colour_texture,
    std::unique_ptr<Texture> depth_texture)
    : RenderTarget(std::move(colour_texture), std::move(depth_texture))
    , handle_(0u)
{
    // create a frame buffer for our target
    ::glGenFramebuffers(1, &handle_);
    expect(check_opengl_error, "could not generate fbo");

    bind(GL_FRAMEBUFFER);

    const auto colour_handle =
        static_cast<OpenGLTexture *>(colour_texture_.get())->handle();

    // set colour texture
    ::glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colour_handle, 0);
    expect(check_opengl_error, "could not attach colour texture");

    const auto depth_handle =
        static_cast<OpenGLTexture *>(depth_texture_.get())->handle();

    // set depth texture
    ::glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_handle, 0);
    expect(check_opengl_error, "could not attach depth texture");

    // check everything worked
    const auto status = ::glCheckFramebufferStatus(GL_FRAMEBUFFER);
    expect(
        status == GL_FRAMEBUFFER_COMPLETE,
        "fbo in invalid state: " + std::to_string(status));

    unbind(GL_FRAMEBUFFER);
}

OpenGLRenderTarget::~OpenGLRenderTarget()
{
    ::glDeleteFramebuffers(1, &handle_);
}

void OpenGLRenderTarget::bind(GLenum target) const
{
    ::glBindFramebuffer(target, handle_);
    expect(check_opengl_error, "could not bind framebuffer");
}

void OpenGLRenderTarget::unbind(GLenum target) const
{
    ::glBindFramebuffer(target, 0u);
    expect(check_opengl_error, "could not bind framebuffer");
}

}
