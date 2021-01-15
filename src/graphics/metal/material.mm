#include "graphics/material.h"

#include <any>
#include <memory>
#include <string>
#include <fstream>

#import <Metal/Metal.h>

#include "core/exception.h"
#include "graphics/buffer_descriptor.h"
#include "graphics/light.h"
#include "graphics/render_graph/compiler.h"
#include "graphics/render_graph/render_graph.h"
#include "core/macos/macos_ios_utility.h"

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
id<MTLFunction> load_function(
    const std::string &source,
    const std::string &function_name)
{
    auto *device = iris::core::utility::metal_device();

    NSError *error = nullptr;

    // load source
    const auto *library = [device newLibraryWithSource:iris::core::utility::string_to_nsstring(source)
                                               options:nullptr
                                                 error:&error];

    if(library == nullptr)
    {
        // an error occurred so parse error and throw
        const std::string error_message{ [[error localizedDescription] UTF8String] };
        throw iris::Exception("failed to load shader: " + error_message);
    }

    return [library newFunctionWithName:iris::core::utility::string_to_nsstring(function_name)];
}

}

namespace iris
{

/**
 * Struct containing implementation specific data.
 */
struct Material::implementation
{
    id<MTLRenderPipelineState> state;
};

Material::Material(
    const RenderGraph &render_graph,
    const BufferDescriptor &vertex_descriptor,
    const std::vector<Light *> &lights)
    : textures_(),
      impl_(std::make_unique<implementation>())
{
    Compiler compiler{render_graph, lights};

    const auto vertex_program = load_function(compiler.vertex_shader(), "vertex_main");
    const auto fragment_program = load_function(compiler.fragment_shader(), "fragment_main");

    const auto vertex_descriptor_handle = std::any_cast<MTLVertexDescriptor*>(vertex_descriptor.native_handle());

    auto *device = core::utility::metal_device();

    // get pipeline state handle
    auto *pipeline_state_descriptor = [[MTLRenderPipelineDescriptor alloc] init];
    [pipeline_state_descriptor setVertexFunction:vertex_program];
    [pipeline_state_descriptor setFragmentFunction:fragment_program];
    pipeline_state_descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
    [pipeline_state_descriptor setDepthAttachmentPixelFormat:MTLPixelFormatDepth32Float];
    [pipeline_state_descriptor setVertexDescriptor:vertex_descriptor_handle];

    impl_->state = [device newRenderPipelineStateWithDescriptor:pipeline_state_descriptor error:nullptr];

    textures_ = compiler.textures();
}

Material::~Material() = default;
Material::Material(Material&&) = default;
Material& Material::operator=(Material&&) = default;

std::any Material::native_handle() const
{
    return {impl_->state};
}

std::vector<Texture *> Material::textures() const
{
    return textures_;
}

}
