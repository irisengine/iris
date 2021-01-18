#include "graphics/render_target.h"

#include <any>
#include <cstdint>
#include <memory>
#include <vector>

#include "core/exception.h"
#include "core/utils.h"
#include "core/window.h"
#include "graphics/opengl/opengl.h"
#include "graphics/pixel_format.h"

namespace iris
{

struct RenderTarget::implementation
{
    GLuint fbo;
};

RenderTarget::RenderTarget(std::uint32_t width, std::uint32_t height)
    : colour_texture_(nullptr)
    , depth_texture_(nullptr)
    , impl_(std::make_unique<implementation>())
{
    // create a frame buffer for our target
    ::glGenFramebuffers(1, &impl_->fbo);
    check_opengl_error("could not generate fbo");

    // bind fbo so we can set textures
    ::glBindFramebuffer(GL_FRAMEBUFFER, impl_->fbo);
    check_opengl_error("could not bind fbo");

    static const auto scale = Window::screen_scale();

    // create backing textures for colour and depth
    colour_texture_ = std::make_unique<Texture>(
        width * scale, height * scale, PixelFormat::RGB);

    const auto colour_handle =
        std::any_cast<std::uint32_t>(colour_texture_->native_handle());

    ::glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colour_handle, 0);
    check_opengl_error("could not attach colour texture");

    depth_texture_ = std::make_unique<Texture>(
        width * scale, height * scale, PixelFormat::DEPTH);

    const auto depth_handle =
        std::any_cast<std::uint32_t>(depth_texture_->native_handle());

    ::glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_handle, 0);
    check_opengl_error("could not attach depth texture");

    // check everything worked
    if (const auto status = ::glCheckFramebufferStatus(GL_FRAMEBUFFER);
        status != GL_FRAMEBUFFER_COMPLETE)
    {
        throw Exception("fbo in invalid state: " + std::to_string(status));
    }

    // we are done, unbind fbo
    ::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check_opengl_error("could not bind default fbo");
}

RenderTarget::~RenderTarget()
{
    ::glDeleteFramebuffers(1, std::addressof(impl_->fbo));
}

std::any RenderTarget::native_handle() const
{
    return {impl_->fbo};
}
Texture *RenderTarget::colour_texture() const
{
    return colour_texture_.get();
}
Texture *RenderTarget::depth_texture() const
{
    return depth_texture_.get();
}
}
