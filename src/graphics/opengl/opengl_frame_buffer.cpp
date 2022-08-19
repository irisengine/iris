////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/opengl_frame_buffer.h"

#include <algorithm>
#include <array>

#include "core/error_handling.h"
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_render_target.h"
#include "graphics/opengl/opengl_texture.h"

namespace
{

void delete_frame_buffer(GLuint handle)
{
    ::glDeleteFramebuffers(1, &handle);
}

}

namespace iris
{

OpenGLFrameBuffer::OpenGLFrameBuffer(
    const OpenGLRenderTarget *colour_target,
    const OpenGLRenderTarget *normal_target,
    const OpenGLRenderTarget *position_target)
    : handle_(0u, delete_frame_buffer)
    , colour_target_(colour_target)
    , normal_target_(normal_target)
    , position_target_(position_target)
{
    // create a frame buffer for our target
    ::glGenFramebuffers(1, &handle_);
    expect(check_opengl_error, "could not generate fbo");

    bind();

    // array of symbols describing the render targets
    std::array<GLenum, 3u> attachments{{GL_NONE, GL_NONE, GL_NONE}};

    if (colour_target != nullptr)
    {
        const auto colour_handle = static_cast<const OpenGLTexture *>(colour_target->colour_texture())->handle();
        ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colour_handle, 0);
        expect(check_opengl_error, "could not attach colour texture");

        attachments[0] = GL_COLOR_ATTACHMENT0;

        // also set depth
        const auto depth_handle = static_cast<const OpenGLTexture *>(colour_target->depth_texture())->handle();
        ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_handle, 0);
        expect(check_opengl_error, "could not attach depth texture");
    }

    if (normal_target != nullptr)
    {
        const auto normal_handle = static_cast<const OpenGLTexture *>(normal_target->colour_texture())->handle();
        ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_handle, 0);
        expect(check_opengl_error, "could not attach normal texture");

        attachments[1] = GL_COLOR_ATTACHMENT1;
    }

    if (position_target != nullptr)
    {
        const auto position_handle = static_cast<const OpenGLTexture *>(position_target->colour_texture())->handle();
        ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, position_handle, 0);
        expect(check_opengl_error, "could not attach position texture");

        attachments[2] = GL_COLOR_ATTACHMENT2;
    }

    // if we set at least one buffer then we need to call glDrawBuffers to tell opengl what we will be rendering to
    if (std::any_of(std::cbegin(attachments), std::cend(attachments), [](auto value) { return value != GL_NONE; }))
    {
        ::glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());
        expect(check_opengl_error, "could not set draw buffers");

        // check everything worked
        const auto status = ::glCheckFramebufferStatus(GL_FRAMEBUFFER);
        expect(status == GL_FRAMEBUFFER_COMPLETE, "fbo in invalid state: " + std::to_string(status));
    }

    unbind();
}

void OpenGLFrameBuffer::bind() const
{
    expect(handle_, "invalid handle");

    ::glBindFramebuffer(GL_FRAMEBUFFER, handle_);
    expect(check_opengl_error, "could not bind framebuffer");
}

void OpenGLFrameBuffer::unbind() const
{
    expect(handle_, "invalid handle");

    ::glBindFramebuffer(GL_FRAMEBUFFER, 0u);
    expect(check_opengl_error, "could not bind framebuffer");
}

const OpenGLRenderTarget *OpenGLFrameBuffer::colour_target() const
{
    return colour_target_;
}

const OpenGLRenderTarget *OpenGLFrameBuffer::normal_target() const
{
    return normal_target_;
}

const OpenGLRenderTarget *OpenGLFrameBuffer::position_target() const
{
    return position_target_;
}

}
