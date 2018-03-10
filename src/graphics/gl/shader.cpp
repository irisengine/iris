#include "gl/shader.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "gl/opengl.hpp"
#include "gl/shader_type.hpp"

namespace eng::gl
{

shader::shader(std::string_view source, shader_type type)
    : shader_(0u)
{
    const auto native_type = (type == shader_type::VERTEX)
        ? GL_VERTEX_SHADER
        : GL_FRAGMENT_SHADER;

    shader_ = ::glCreateShader(native_type);
    gl::check_opengl_error("could not create vertex shader");

    const auto shader_c_str = source.data();

    ::glShaderSource(shader_, 1, &shader_c_str, nullptr);
    eng::gl::check_opengl_error("could not set shader source");

    ::glCompileShader(shader_);

    std::int32_t shader_param = 0;

    ::glGetShaderiv(shader_, GL_COMPILE_STATUS, &shader_param);
    eng::gl::check_opengl_error("could not get shader parameter");

    // if shader failed to compile then get the opengl error
    if(shader_param != GL_TRUE)
    {
        ::glGetShaderiv(shader_, GL_INFO_LOG_LENGTH, &shader_param);
        eng::gl::check_opengl_error("could not get shader log length");

        if(shader_param == 0)
        {
            throw std::runtime_error("shader compilation failed: no log");
        }
        else
        {
            std::vector<char> error_log(shader_param);

            // get opengl error log
            std::int32_t log_length = 0;
            ::glGetShaderInfoLog(
                shader_,
                static_cast<std::int32_t>(error_log.size()),
                &log_length,
                error_log.data());
            eng::gl::check_opengl_error("failed to get error log");

            // convert to string and throw
            const std::string error(error_log.data(), log_length);
            throw std::runtime_error("shader compilation failed: " + error);
        }
    }
}

shader::~shader()
{
    ::glDeleteShader(shader_);
}

shader::shader(shader &&other) noexcept
    : shader_(0u)
{
    std::swap(shader_, other.shader_);
}

shader& shader::operator=(shader &&other) noexcept
{
    // create a new shader object to 'steal' the internal state of the supplied
    // object then swap
    // this ensures that the current shader is correctly deleted at the end
    // of this call
    shader new_shader{ std::move(other) };
    std::swap(shader_, new_shader.shader_);

    return *this;
}

std::uint32_t shader::native_handle() const noexcept
{
    return shader_;
}

}

