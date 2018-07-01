#include "metal/material_implementation.hpp"

#include <any>
#include <cstdint>
#include <string>

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#include "exception.hpp"

namespace
{

NSString* string_to_nsstring(const std::string &str)
{
    return [NSString stringWithUTF8String:str.c_str()];
}

id<MTLFunction> load_function(
    id<MTLDevice> device,
    const std::string &source,
    const std::string &function_name)
{
    NSError *error = nullptr;

    const auto *vertex_library =
        [device newLibraryWithSource:string_to_nsstring(source)
                             options:nullptr
                               error:&error];

    if(vertex_library == nullptr)
    {
        const std::string error_message{ [[error localizedDescription] UTF8String] };
        throw eng::exception("failed to load shader: " + error_message);
    }

    return [vertex_library newFunctionWithName:string_to_nsstring(function_name)];
}

}

namespace eng
{

material_implementation::material_implementation(
    const std::string &vertex_shader_source,
    const std::string &fragment_shader_source)
    : pipeline_state_()
{
    // get metal device handle
    auto *device =
        ::CGDirectDisplayCopyCurrentMetalDevice(::CGMainDisplayID());

    // load shaders and entry functions
    const auto vertex_program = load_function(device, vertex_shader_source, "vertex_main");
    const auto fragment_program = load_function(device, fragment_shader_source, "fragment_main");

    // create and setup a metal pipeline state descriptor
    auto *pipeline_state_descriptor = [[MTLRenderPipelineDescriptor alloc] init];
    [pipeline_state_descriptor setVertexFunction:vertex_program];
    [pipeline_state_descriptor setFragmentFunction:fragment_program];
    pipeline_state_descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    [pipeline_state_descriptor setDepthAttachmentPixelFormat:MTLPixelFormatDepth32Float];

    pipeline_state_ =
        [device newRenderPipelineStateWithDescriptor:pipeline_state_descriptor error:nullptr];
}

std::any material_implementation::native_handle() const noexcept
{
    return pipeline_state_;
}

}

