////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#include "graphics/mesh.h"
#include "graphics/primitive_type.h"

namespace iris
{

/**
 * Abstract class representing a renderable entity.
 */
class RenderEntity
{
  public:
    /**
     * Construct a RenderEntity.
     *
     * @param mesh
     *   Mesh to render.
     *
     * @param primitive_type
     *   Primitive type of underlying mesh.
     */
    RenderEntity(const Mesh *mesh, PrimitiveType primitive_type = PrimitiveType::TRIANGLES);

    virtual ~RenderEntity() = default;

    RenderEntity(const RenderEntity &) = delete;
    RenderEntity &operator=(const RenderEntity &) = delete;
    RenderEntity(RenderEntity &&) = default;
    RenderEntity &operator=(RenderEntity &&) = default;

    /**
     * Get if entity will be rendered with transparency.
     *
     * @returns
     *   True if entity will have transparency, otherwise false.
     */
    virtual bool has_transparency() const = 0;

    /**
     * Get all Mesh for this entity.
     *
     * @returns
     *   Mesh.
     */
    const Mesh *mesh() const;

    /**
     * Returns whether the object should be rendered as a wireframe.
     *
     * @returns
     *   True if should be rendered as a wireframe, false otherwise.
     */
    bool should_render_wireframe() const;

    /**
     * Sets whether the object should be rendered as a wireframe.
     *
     * @param wrireframe
     *   True if should be rendered as a wireframe, false otherwise.
     */
    void set_wireframe(const bool wireframe);

    /**
     * Get primitive type.
     *
     * @returns
     *   Primitive type.
     */
    PrimitiveType primitive_type() const;

    /**
     * Get (optional) name.
     *
     * @return
     *  Name of entity, empty string if not set.
     */
    std::string name() const;

    /**
     * Set the name of the entity.
     *
     * @param name
     *   New name.
     */
    void set_name(std::string_view name);

    /**
     * Can this entity have shadows rendered on it.
     *
     * @returns
     *   True if shadows should be rendered, false otherwise.
     */
    bool receive_shadow() const;

    /**
     * Set whether this object can have shadows rendered on it.
     *
     * @param receive_shadow
     *   New receive shadow option.
     */
    void set_receive_shadow(bool receive_shadow);

  protected:
    /** Mesh to render. */
    const Mesh *mesh_;

    /** Whether the object should be rendered as a wireframe. */
    bool wireframe_;

    /** Primitive type. */
    PrimitiveType primitive_type_;

    /** Optional name (default is empty string). */
    std::string name_;

    /** Should object render shadows. */
    bool receive_shadow_;
};

}
