////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/metal/metal_material.h"

#include <any>
#include <fstream>
#include <memory>
#include <string>

#import <Metal/Metal.h>

#include "core/error_handling.h"
#include "core/macos/macos_ios_utility.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/mesh.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/render_graph/shader_compiler.h"
#include "log/log.h"

namespace
{

/**
 * Helper function to load a metal shader and get a handle to a function.
 *
 * @param source
 *   Source of shader.
 *
 * @param function_name
 *   Name of function to get handle to.
 *
 * @returns
 *   Handle to function in loaded source.
 */
id<MTLFunction> load_function(const std::string &source, const std::string &function_name)
{
    auto *device = iris::core::utility::metal_device();

    NSError *error = nullptr;

    // load source
    const auto *library = [device newLibraryWithSource:iris::core::utility::string_to_nsstring(source)
                                               options:nullptr
                                                 error:&error];

    if (library == nullptr)
    {
        auto line_number = 1u;

        std::istringstream strm{source};
        for (std::string line; std::getline(strm, line);)
        {
            LOG_ENGINE_ERROR("metal_material", "{}: {}", line_number, line);
            ++line_number;
        }
        // an error occurred so parse error and throw
        const std::string error_message{[[error localizedDescription] UTF8String]};

        LOG_ENGINE_ERROR("metal_material", "{}", error_message);

        throw iris::Exception("failed to load shader: " + error_message);
    }

    return [library newFunctionWithName:iris::core::utility::string_to_nsstring(function_name)];
}

}

namespace iris
{

MetalMaterial::MetalMaterial(
    const RenderGraph *render_graph,
    MTLVertexDescriptor *descriptors,
    LightType light_type,
    bool render_to_normal_target,
    bool render_to_position_target)
    : pipeline_state_()
{
    ShaderCompiler compiler{
        ShaderLanguage::MSL, render_graph, light_type, render_to_normal_target, render_to_position_target};

    const auto vertex_program = load_function(compiler.vertex_shader(), "vertex_main");
    const auto fragment_program = load_function(compiler.fragment_shader(), "fragment_main");

    auto *device = core::utility::metal_device();

    // get pipeline state handle
    auto *pipeline_state_descriptor = [[MTLRenderPipelineDescriptor alloc] init];
    [pipeline_state_descriptor setVertexFunction:vertex_program];
    [pipeline_state_descriptor setFragmentFunction:fragment_program];
    pipeline_state_descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA16Float;
    [pipeline_state_descriptor setDepthAttachmentPixelFormat:MTLPixelFormatDepth32Float];
    [pipeline_state_descriptor setVertexDescriptor:descriptors];

    if (render_to_normal_target)
    {
        pipeline_state_descriptor.colorAttachments[1].pixelFormat = MTLPixelFormatRGBA16Float;
    }

    if (render_to_position_target)
    {
        pipeline_state_descriptor.colorAttachments[2].pixelFormat = MTLPixelFormatRGBA16Float;
    }

    // set blend mode based on light
    // ambient is always rendered first (no blending)
    // directional and point are always rendered after (blending)
    switch (light_type)
    {
        case LightType::AMBIENT:
            [[[pipeline_state_descriptor colorAttachments] objectAtIndexedSubscript:0] setBlendingEnabled:false];
            break;
        case LightType::DIRECTIONAL:
        case LightType::POINT:
            [[[pipeline_state_descriptor colorAttachments] objectAtIndexedSubscript:0] setBlendingEnabled:true];
            pipeline_state_descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
            pipeline_state_descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
            pipeline_state_descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;
            break;
    }

    NSError *error = nullptr;

    pipeline_state_ = [device newRenderPipelineStateWithDescriptor:pipeline_state_descriptor error:&error];

    expect(error == nullptr, "failed to create pipeline state");
}

id<MTLRenderPipelineState> MetalMaterial::pipeline_state() const
{
    return pipeline_state_;
}

}
