////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

#include "graphics/opengl/opengl.h"
#include "graphics/sampler.h"

namespace iris
{

/**
 * Implementation of Sampler for OpenGL.
 */
class OpenGLSampler : public Sampler
{
  public:
    /**
     * Create a new OpenGLSampler.
     *
     * @param descriptor
     *   Description of sampler parameters.
     *
     * @param index
     *   Index into the global array of all allocated samplers.
     */
    OpenGLSampler(const SamplerDescriptor &descriptor, std::uint32_t index);
    ~OpenGLSampler();

    /**
     * Get OpenGL handle to sample.
     *
     * @returns
     *   OpenGL handle.
     */
    GLuint handle() const;

  private:
    /** OpenGL handle to sampler object. */
    GLuint handle_;
};

}
