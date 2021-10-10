////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#include "graphics/opengl/opengl.h"
#include "graphics/shader_type.h"

namespace iris
{

/**
 * Class encapsulating an opengl shader.
 */
class OpenGLShader
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
    OpenGLShader(const std::string &source, ShaderType type);

    /**
     * Destructor, performs opengl cleanup.
     */
    ~OpenGLShader();

    /**
     * Move constructor, steals the state from the moved-in object.
     *
     * @param other
     *   Object to take state from. Do not use after this call.
     */
    OpenGLShader(OpenGLShader &&other);

    /**
     * Move operator, steals the state from the moved-in object.
     *
     * @param other
     *   Object to take state from. Do not use after this call.
     */
    OpenGLShader &operator=(OpenGLShader &&);

    /** Disabled */
    OpenGLShader(const OpenGLShader &) = delete;
    OpenGLShader &operator=(const OpenGLShader &) = delete;

    /**
     * Get the native opengl handle.
     *
     * @returns native opengl handle.
     */
    GLuint native_handle() const;

  private:
    /** Opengl shader object. */
    GLuint shader_;
};

}
