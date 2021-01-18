#pragma once

#include <string>

#include "graphics/opengl/shader_type.h"

namespace iris
{

/**
 * Class encapsulating an opengl shader.
 */
class shader
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
    shader(const std::string &source, shader_type type);

    /**
     * Destructor, performs opengl cleanup.
     */
    ~shader();

    /**
     * Move constructor, steals the state from the moved-in object.
     *
     * @param other
     *   Object to take state from. Do not use after this call.
     */
    shader(shader &&other);

    /**
     * Move operator, steals the state from the moved-in object.
     *
     * @param other
     *   Object to take state from. Do not use after this call.
     */
    shader &operator=(shader &&);

    /** Disabled */
    shader(const shader &) = delete;
    shader &operator=(const shader &) = delete;

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
