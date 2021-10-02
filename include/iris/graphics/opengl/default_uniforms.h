#pragma once

#include <vector>

#include "graphics/opengl/opengl_uniform.h"

namespace iris
{

/**
 * Struct encapsulating all the default required uniforms for OpenGL rendering.
 */
struct DefaultUniforms
{
    /**
     * Construct a new DefaultUniforms.
     *
     * @param projection
     *   Projection uniform.
     *
     * @param view
     *   View uniform.
     *
     * @param model
     *   Model uniform.
     *
     * @param normal_matrix
     *   Normal matrix uniform.
     *
     * @param light_colour
     *   Colour of the light.
     *
     * @param light_position
     *   Position data of the light.
     *
     * @param light_attenuation
     *   Attenuation data of the light.
     *
     * @param shadow_map
     *   Shadow map uniform.
     *
     * @param light_projection
     *   Light camera projection uniform.
     *
     * @param light_view
     *   Light camera view uniform.
     *
     * @param bones
     *   Bones uniform.
     */
    DefaultUniforms(
        OpenGLUniform projection,
        OpenGLUniform view,
        OpenGLUniform model,
        OpenGLUniform normal_matrix,
        OpenGLUniform light_colour,
        OpenGLUniform light_position,
        OpenGLUniform light_attenuation,
        OpenGLUniform shadow_map,
        OpenGLUniform light_projection,
        OpenGLUniform light_view,
        OpenGLUniform bones)
        : projection(projection)
        , view(view)
        , model(model)
        , normal_matrix(normal_matrix)
        , light_colour(light_colour)
        , light_position(light_position)
        , light_attenuation(light_attenuation)
        , shadow_map(shadow_map)
        , light_projection(light_projection)
        , light_view(light_view)
        , bones(bones)
        , textures()
    {
    }

    /** Projection uniform. */
    OpenGLUniform projection;

    /** View uniform. */
    OpenGLUniform view;

    /** Model uniform. */
    OpenGLUniform model;

    /** Normal matrix uniform. */
    OpenGLUniform normal_matrix;

    /** Colour of the light. */
    OpenGLUniform light_colour;

    /** Position data of the light. */
    OpenGLUniform light_position;

    /** Attenuation data of the light. */
    OpenGLUniform light_attenuation;

    /** Shadow map uniform. */
    OpenGLUniform shadow_map;

    /** Light camera projection uniform. */
    OpenGLUniform light_projection;

    /** Light camera view uniform. */
    OpenGLUniform light_view;

    /** Bones uniform. */
    OpenGLUniform bones;

    /** Collection of texture uniforms. */
    std::vector<OpenGLUniform> textures;
};

}
