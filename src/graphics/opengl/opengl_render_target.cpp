#include "graphics/opengl/opengl_render_target.h"

#include <cstdint>
#include <memory>

#include "core/exception.h"
#include "core/root.h"
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_texture.h"
#include "graphics/opengl/opengl_texture_manager.h"
#include "graphics/pixel_format.h"
#include "graphics/texture_manager.h"

namespace
{

GLuint create_fbo(GLuint colour_handle, GLuint depth_handle, GLenum target)
{
    GLuint handle = 0u;

    // create a frame buffer for our target
    ::glGenFramebuffers(1, &handle);
    iris::check_opengl_error("could not generate fbo");

    ::glBindFramebuffer(GL_FRAMEBUFFER, handle);
    iris::check_opengl_error("could not bind framebuffer");

    // set colour texture
    ::glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, colour_handle, 0);
    iris::check_opengl_error("could not attach colour texture");

    // set depth texture
    ::glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, depth_handle, 0);
    iris::check_opengl_error("could not attach depth texture");

    // check everything worked
    if (const auto status = ::glCheckFramebufferStatus(GL_FRAMEBUFFER);
        status != GL_FRAMEBUFFER_COMPLETE)
    {
        throw iris::Exception(
            "fbo in invalid state: " + std::to_string(status));
    }

    ::glBindFramebuffer(GL_FRAMEBUFFER, 0u);
    iris::check_opengl_error("could not bind framebuffer");

    return handle;
}

}

namespace iris
{

OpenGLRenderTarget::OpenGLRenderTarget(
    std::unique_ptr<Texture> colour_texture,
    std::unique_ptr<Texture> depth_texture,
    std::uint32_t samples)
    : RenderTarget(std::move(colour_texture), std::move(depth_texture))
    , handle_(0u)
    , multisample_handle_(0u)
    , multisample_colour_texture_(nullptr)
    , multisample_depth_texture_(nullptr)
    , samples_(samples)
{
    handle_ = create_fbo(
        static_cast<OpenGLTexture *>(colour_texture_.get())->handle(),
        static_cast<OpenGLTexture *>(depth_texture_.get())->handle(),
        GL_TEXTURE_2D);

    if (samples_ > 0u)
    {
        auto &tex_man =
            static_cast<OpenGLTextureManager &>(Root::texture_manager());

        multisample_colour_texture_ = std::make_unique<OpenGLTexture>(
            DataBuffer{},
            colour_texture_->width(),
            colour_texture_->height(),
            colour_texture_->pixel_format(),
            tex_man.next_id(),
            samples_);

        multisample_depth_texture_ = std::make_unique<OpenGLTexture>(
            DataBuffer{},
            depth_texture_->width(),
            depth_texture_->height(),
            depth_texture_->pixel_format(),
            tex_man.next_id(),
            samples_);

        multisample_handle_ = create_fbo(
            multisample_colour_texture_->handle(),
            multisample_depth_texture_->handle(),
            GL_TEXTURE_2D_MULTISAMPLE);
    }
}

OpenGLRenderTarget::~OpenGLRenderTarget()
{
    ::glDeleteFramebuffers(1, &handle_);
}

void OpenGLRenderTarget::bind(GLenum target) const
{
    ::glBindFramebuffer(target, multisample_handle_);
    check_opengl_error("could not bind framebuffer");
}

void OpenGLRenderTarget::unbind(GLenum target) const
{
    ::glBindFramebuffer(target, 0u);
    check_opengl_error("could not bind framebuffer");
}

void OpenGLRenderTarget::multisample_resolve() const
{
    if (samples_ > 0)
    {
        ::glBindFramebuffer(GL_READ_FRAMEBUFFER, multisample_handle_);
        check_opengl_error("could not bind read frame buffer");

        ::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, handle_);
        check_opengl_error("could not bind draw frame buffer");

        const auto width = colour_texture_->width();
        const auto height = colour_texture_->height();

        ::glBlitFramebuffer(
            0,
            0,
            width,
            height,
            0,
            0,
            width,
            height,
            GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
            GL_NEAREST);
        check_opengl_error("failed to blit");
    }
}

}
