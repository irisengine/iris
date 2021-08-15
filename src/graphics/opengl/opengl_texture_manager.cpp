#include "graphics/opengl/opengl_texture_manager.h"

#include <cstdint>
#include <memory>
#include <stack>

#include "core/data_buffer.h"
#include "core/exception.h"
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_texture.h"
#include "graphics/pixel_format.h"
#include "graphics/texture_manager.h"

namespace iris
{

OpenGLTextureManager::OpenGLTextureManager()
    : TextureManager()
    , id_pool_()
{
    for (auto i = 79u; i > 0u; --i)
    {
        id_pool_.emplace(GL_TEXTURE0 + i);
    }
}

GLuint OpenGLTextureManager::next_id()
{
    if (id_pool_.empty())
    {
        throw Exception("texture id pool empty");
    }

    const auto id = id_pool_.top();
    id_pool_.pop();

    return id;
}

void OpenGLTextureManager::return_id(GLuint id)
{
    id_pool_.emplace(id);
}

std::unique_ptr<Texture> OpenGLTextureManager::create(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_format)
{
    return std::make_unique<OpenGLTexture>(
        data, width, height, pixel_format, next_id());
}

void OpenGLTextureManager::destroy(Texture *texture)
{
    // return id of texture to the pool
    return_id(static_cast<OpenGLTexture *>(texture)->id());
}

}