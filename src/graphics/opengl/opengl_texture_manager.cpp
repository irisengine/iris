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
#include "core/resource_manager.h"
#include "graphics/opengl/opengl.h"
#include "graphics/opengl/opengl_cube_map.h"
#include "graphics/opengl/opengl_sampler.h"
#include "graphics/opengl/opengl_texture.h"
#include "graphics/sampler.h"
#include "graphics/texture_manager.h"
#include "graphics/texture_usage.h"


namespace iris
{

OpenGLTextureManager::OpenGLTextureManager(ResourceManager &resource_manager)
    : TextureManager(resource_manager)
{
}

std::unique_ptr<Texture> OpenGLTextureManager::do_create(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    const Sampler *sampler,
    TextureUsage usage,
    std::uint32_t index)
{
    return std::make_unique<OpenGLTexture>(data, width, height, sampler, usage, index);
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
    const Sampler *sampler,
    std::uint32_t index)
{
    return std::make_unique<OpenGLCubeMap>(
        right_data, left_data, top_data, bottom_data, near_data, far_data, width, height, sampler, index);
}

std::unique_ptr<Sampler> OpenGLTextureManager::do_create(const SamplerDescriptor &descriptor, std::uint32_t index)
{
    return std::make_unique<OpenGLSampler>(descriptor, index);
}

}
