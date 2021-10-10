////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "core/matrix4.h"
#include "graphics/opengl/opengl.h"

namespace iris
{

/**
 * This class encapsulates an OpenGL uniform and provides methods for uploading
 * data. It is the callers responsability to ensure the correct sized data is
 * written to the uniform.
 *
 * Note that in the case where ensure_exists is false it is still valid to call
 * set_value.
 */
class OpenGLUniform
{
  public:
    /**
     * Construct a new OpenGLUniform.
     *
     * @param program
     *   OpenGL program uniform is for.
     *
     * @param name
     *   The name of the uniform.
     *
     * @param ensure_exists
     *   If true then an exception will be thrown if the uniform does not
     *   exists, else construction continues as normal.
     */
    OpenGLUniform(
        GLuint program,
        const std::string &name,
        bool ensure_exists = true);

    /**
     * Set a matrix value for the uniform.
     *
     * @param value
     *   The value to set.
     */
    void set_value(const Matrix4 &value) const;

    /**
     * Set an array of matrix values for the uniform.
     *
     * @param value
     *   The value to set.
     */
    void set_value(const std::vector<Matrix4> &value) const;

    /**
     * Set an array of float values for the uniform.
     *
     * @param value
     *   The value to set.
     */
    void set_value(const std::array<float, 4u> &value) const;

    /**
     * Set an array of float values for the uniform.
     *
     * @param value
     *   The value to set.
     */
    void set_value(const std::array<float, 3u> &value) const;

    /**
     * Set an integer value for the uniform.
     *
     * @param value
     *   The value to set.
     */
    void set_value(std::int32_t value) const;

  private:
    /** OpenGL location of uniform. */
    GLint location_;
};

}
