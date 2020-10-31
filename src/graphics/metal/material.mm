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
    const std::string &source,
    const std::string &function_name)
{
    auto *device = iris::platform::utility::metal_device();

    NSError *error = nullptr;

    // load source
    const auto *library = [device newLibraryWithSource:iris::platform::utility::string_to_nsstring(source)
                                               options:nullptr
                                                 error:&error];

    if(library == nullptr)
    {
        // an error occurred so parse error and throw
        const std::string error_message{ [[error localizedDescription] UTF8String] };
        throw iris::Exception("failed to load shader: " + error_message);
    }

    return [library newFunctionWithName:iris::platform::utility::string_to_nsstring(function_name)];
}

}

namespace iris
{

/**
 * Struct containing implementation specific data.
 */
struct Material::implementation
{
    id<MTLFunction> vertex_program;
    id<MTLFunction> fragment_program;
};

Material::Material(
    const std::string &vertex_shader_source,
    const std::string &fragment_shader_source)
    : impl_(std::make_unique<implementation>())
{
    // load shaders and entry functions
    impl_->vertex_program = load_function(vertex_shader_source, "vertex_main");
    impl_->fragment_program = load_function(fragment_shader_source, "fragment_main");
}

/** Default */
Material::~Material() = default;
Material::Material(Material&&) = default;
Material& Material::operator=(Material&&) = default;

std::any Material::native_handle() const
{
    return std::any{ std::make_tuple(impl_->vertex_program, impl_->fragment_program) };
}

}
