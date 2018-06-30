#include "material.hpp"

#include <any>
#include <string>

namespace eng
{

material::material(
    const std::string &vertex_shader_source,
    const std::string &fragment_shader_source)
    : impl_(vertex_shader_source, fragment_shader_source)
{ }

std::any material::native_handle() const noexcept
{
    return impl_.native_handle();
}

}

