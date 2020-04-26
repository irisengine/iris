#include "graphics/material.h"

#include <any>
#include <memory>
#include <string>

#import <Metal/Metal.h>

#include "core/exception.h"
#include "platform/macos/macos_ios_utility.h"

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
    id<MTLDevice> device,
    const std::string &source,
    const std::string &function_name)
{
    NSError *error = nullptr;

    // load source
    const auto *library = [device newLibraryWithSource:eng::platform::utility::string_to_nsstring(source)
                                               options:nullptr
                                                 error:&error];

    if(library == nullptr)
    {
        // an error occurred so parse error and throw
        const std::string error_message{ [[error localizedDescription] UTF8String] };
        throw eng::Exception("failed to load shader: " + error_message);
    }

    return [library newFunctionWithName:eng::platform::utility::string_to_nsstring(function_name)];
}

}

namespace eng
{

/**
 * Struct containing implementation specific data.
 */
struct Material::implementation
{
    /** Simple constructor which takes a value for each member. */
    implementation(id<MTLRenderPipelineState> pipeline_state)
        : pipeline_state(pipeline_state)
    { }

    /** Handle to metal pipeline state. */
    id<MTLRenderPipelineState> pipeline_state;
};

Material::Material(
    const std::string &vertex_shader_source,
    const std::string &fragment_shader_source)
    : impl_(nullptr)
{
    auto *device = eng::platform::utility::metal_device();
    
    // load shaders and entry functions
    const auto vertex_program = load_function(device, vertex_shader_source, "vertex_main");
    const auto fragment_program = load_function(device, fragment_shader_source, "fragment_main");

    // create and setup a metal pipeline state descriptor
    auto *pipeline_state_descriptor = [[MTLRenderPipelineDescriptor alloc] init];
    [pipeline_state_descriptor setVertexFunction:vertex_program];
    [pipeline_state_descriptor setFragmentFunction:fragment_program];
    pipeline_state_descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    [pipeline_state_descriptor setDepthAttachmentPixelFormat:MTLPixelFormatInvalid];

    auto *pipeline_state =
        [device newRenderPipelineStateWithDescriptor:pipeline_state_descriptor error:nullptr];

    impl_ = std::make_unique<implementation>(pipeline_state);
}

/** Default */
Material::~Material() = default;
Material::Material(Material&&) = default;
Material& Material::operator=(Material&&) = default;

std::any Material::native_handle() const
{
    return std::any{ impl_->pipeline_state };
}

}

