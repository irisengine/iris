////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/opengl/opengl_texture_manager.h"

#include <cstdint>
#include <memory>
#include <stack>

#include "core/data_buffer.h"
#include "core/error_handling.h"
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_cube_map.h"
#include "graphics/opengl/opengl_texture.h"
#include "graphics/texture_manager.h"
#include "graphics/texture_usage.h"

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
    expect(!id_pool_.empty(), "texture id pool empty");

    const auto id = id_pool_.top();
    id_pool_.pop();

    return id;
}

void OpenGLTextureManager::return_id(GLuint id)
{
    id_pool_.emplace(id);
}

std::unique_ptr<Texture> OpenGLTextureManager::do_create(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    TextureUsage usage,
    std::uint32_t index)
{
    return std::make_unique<OpenGLTexture>(data, width, height, usage, index, next_id());
}

std::unique_ptr<CubeMap> OpenGLTextureManager::do_create(
    const DataBuffer &right_data,
    const DataBuffer &left_data,
    const DataBuffer &top_data,
    const DataBuffer &bottom_data,
    const DataBuffer &near_data,
    const DataBuffer &far_data,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t index)
{
    return std::make_unique<OpenGLCubeMap>(
        right_data, left_data, top_data, bottom_data, near_data, far_data, width, height, index, next_id());
}

void OpenGLTextureManager::destroy(Texture *texture)
{
    // return id of texture to the pool
    return_id(static_cast<OpenGLTexture *>(texture)->id());
}

void OpenGLTextureManager::destroy(CubeMap *cube_map)
{
    // return id of texture to the pool
    return_id(static_cast<OpenGLCubeMap *>(cube_map)->id());
}

}
