#pragma once

#include <utility>

#include "gl/material.hpp"
#include "gl/vertex_data.hpp"
#include "gl/vertex_state.hpp"

namespace eng::gl
{

/**
 * Struct encapsulating opengl data needed to render triangles.
 */
struct entity_data
{
    /**
     * Constuct a new opengl_entity_data.
     *
     * @param state
     *   Vertex state to use for rendering.
     *
     * @param data
     *   Vertex data to render.
     *
     * @param material
     *  Material to render with.
     */
    entity_data(
        gl::vertex_state &&state,
        gl::vertex_data &&data,
        gl::material &&material)
        : state(std::move(state)),
          data(std::move(data)),
          mat(std::move(material))
    { }

    /** Vertex state. */
    gl::vertex_state state;

    /** Vertex data. */
    gl::vertex_data data;

    /** Material to render with. */
    gl::material mat;
};

}

