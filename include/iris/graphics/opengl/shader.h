#pragma once

#include <string>

#include "graphics/opengl/shader_type.h"

namespace iris
{

/**
 * Class encapsulating an opengl shader.
 */
class Shader
{
  public:
    /**
     * Construct a new shader.
     *
     * @param source
     *   Source of the opengl shader.
     *
     * @param type
     *   The type of shader.
     */
    Shader(const std::string &source, ShaderType type);

    /**
     * Destructor, performs opengl cleanup.
     */
    ~Shader();

    /**
     * Move constructor, steals the state from the moved-in object.
     *
     * @param other
     *   Object to take state from. Do not use after this call.
     */
    Shader(Shader &&other);

    /**
     * Move operator, steals the state from the moved-in object.
     *
     * @param other
     *   Object to take state from. Do not use after this call.
     */
    Shader &operator=(Shader &&);

    /** Disabled */
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    /**
     * Get the native opengl handle.
     *
     * @returns native opengl handle.
     */
    std::uint32_t native_handle() const;

  private:
    /** Opengl shader object. */
    std::uint32_t shader_;
};

}
