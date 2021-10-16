////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graphics/lights/light.h"
#include "graphics/lights/light_type.h"
#include "graphics/material.h"
#include "graphics/render_command_type.h"
#include "graphics/render_entity.h"
#include "graphics/render_pass.h"
#include "graphics/render_target.h"

namespace iris
{

/**
 * A RenderCommand represents an instruction to a Renderer. It is possible to
 * create a queue of these commands such that when executed by a Renderer the
 * desired output is presented to a Window. It is expected that each Renderer
 * implementation interprets these commands in such a way that the output is
 * always the same, therefore the command queue is agnostic to the graphics API.
 *
 * This class is just a grab-bag of pointers to various rendering objects. The
 * type will infer which are valid for any given command (can assume nullptr if
 * not valid for a given type).
 *
 * This is an internal class and a queue is built from RenderPass objects by a
 * Renderer.
 *
 * Note also that this class can violate the "correct by construction" paradigm
 * in that:
 *  - It has a zero argument constructor
 *  - Every member has a getter/setter and no validation is performed against
 *    the type.
 *
 * This is acceptable because:
 *  1. It's an internal class so the engine is aware of the limitations.
 *  2. It makes creating a queue easier as we can construct one and then just
 *     use the setters to update the fields as we need to. This is apposed to
 *     having to recreate the full state of each command every time we need a
 *     new one.
 */
class RenderCommand
{
  public:
    /**
     * Construct a new RenderCommand. Default type is PASS_START
     */
    RenderCommand();

    /**
     * Constructor a new RenderCommand.
     *
     * @param type
     *   Command type.
     *
     * @param render_pass
     *   Pointer to RenderPass.
     *
     * @param material
     *   Pointer to Material.
     *
     * @param render_entity
     *   Pointer to RenderEntity.
     *
     * @param shadow_map
     *   Pointer to shadow map RenderTarget.
     *
     * @param light
     *   Pointer to light.
     */
    RenderCommand(
        RenderCommandType type,
        const RenderPass *render_pass,
        const Material *material,
        const RenderEntity *render_entity,
        const RenderTarget *shadow_map,
        const Light *light);

    /**
     * Get command type.
     *
     * @returns
     *   Command type.
     */
    RenderCommandType type() const;

    /**
     * Set command type.
     *
     * @param type
     *   New command type.
     */
    void set_type(RenderCommandType type);

    /**
     * Get pointer to RenderPass.
     *
     * @returns
     *   Pointer to RenderPass.
     */
    const RenderPass *render_pass() const;

    /**
     * Set RenderPass.
     *
     * @param render_pass
     *   New RenderPass.
     */
    void set_render_pass(const RenderPass *render_pass);

    /**
     * Get pointer to Material.
     *
     * @returns
     *   Pointer to Material.
     */
    const Material *material() const;

    /**
     * Set Material.
     *
     * @param material
     *   New Material.
     */
    void set_material(const Material *material);

    /**
     * Get pointer to RenderEntity.
     *
     * @returns
     *   Pointer to RenderEntity.
     */
    const RenderEntity *render_entity() const;

    /**
     * Set RenderEntity.
     *
     * @param render_entity
     *   New Material.
     */
    void set_render_entity(const RenderEntity *render_entity);

    /**
     * Get pointer to Light.
     *
     * @returns
     *   Pointer to Light.
     */
    const Light *light() const;

    /**
     * Set Light.
     *
     * @param light
     *   New Light.
     */
    void set_light(const Light *light);

    /**
     * Get pointer to shadow map RenderTarget.
     *
     * @returns
     *   Pointer to shadow map RenderTarget.
     */
    const RenderTarget *shadow_map() const;

    /**
     * Set shadow map RenderTarget.
     *
     * @param light
     *   New shadow map RenderTarget.
     */
    void set_shadow_map(const RenderTarget *shadow_map);

    /**
     * Equality operator.
     *
     * @param other
     *   RenderCommand to compare with this.
     *
     * @returns
     *   True if objects are equal, otherwise false.
     */
    bool operator==(const RenderCommand &other) const;

    /**
     * Inequality operator.
     *
     * @param other
     *   RenderCommand to compare with this.
     *
     * @returns
     *   True if objects are not equal, otherwise false.
     */
    bool operator!=(const RenderCommand &other) const;

  private:
    /** Command type. */
    RenderCommandType type_;

    /** Pointer to RenderPass for command. */
    const RenderPass *render_pass_;

    /** Pointer to Material for command. */
    const Material *material_;

    /** Pointer to RenderEntity for command. */
    const RenderEntity *render_entity_;

    /** Pointer to shadow map RenderTarget for command. */
    const RenderTarget *shadow_map_;

    /** Pointer to Light for command. */
    const Light *light_;
};

}
