////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_command.h"

#include "graphics/lights/light.h"
#include "graphics/lights/light_type.h"
#include "graphics/material.h"
#include "graphics/render_command_type.h"
#include "graphics/render_entity.h"
#include "graphics/render_pass.h"
#include "graphics/render_target.h"

namespace iris
{

RenderCommand::RenderCommand()
    : type_(RenderCommandType::PASS_START)
    , render_pass_(nullptr)
    , material_(nullptr)
    , render_entity_(nullptr)
    , shadow_map_(nullptr)
    , light_(nullptr)
{
}

RenderCommand::RenderCommand(
    RenderCommandType type,
    const RenderPass *render_pass,
    const Material *material,
    const RenderEntity *render_entity,
    const RenderTarget *shadow_map,
    const Light *light)
    : type_(type)
    , render_pass_(render_pass)
    , material_(material)
    , render_entity_(render_entity)
    , shadow_map_(shadow_map)
    , light_(light)
{
}

RenderCommandType RenderCommand::type() const
{
    return type_;
}

void RenderCommand::set_type(RenderCommandType type)
{
    type_ = type;
}

const RenderPass *RenderCommand::render_pass() const
{
    return render_pass_;
}

void RenderCommand::set_render_pass(const RenderPass *render_pass)
{
    render_pass_ = render_pass;
}

const Material *RenderCommand::material() const
{
    return material_;
}

void RenderCommand::set_material(const Material *material)
{
    material_ = material;
}

const RenderEntity *RenderCommand::render_entity() const
{
    return render_entity_;
}

void RenderCommand::set_render_entity(const RenderEntity *render_entity)
{
    render_entity_ = render_entity;
}

const Light *RenderCommand::light() const
{
    return light_;
}

void RenderCommand::set_light(const Light *light)
{
    light_ = light;
}

const RenderTarget *RenderCommand::shadow_map() const
{
    return shadow_map_;
}

void RenderCommand::set_shadow_map(const RenderTarget *shadow_map)
{
    shadow_map_ = shadow_map;
}

}
