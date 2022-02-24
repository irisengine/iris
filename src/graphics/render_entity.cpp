////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_entity.h"

#include <string>
#include <string_view>

#include "graphics/mesh.h"
#include "graphics/primitive_type.h"

namespace iris
{

RenderEntity::RenderEntity(const Mesh *mesh, PrimitiveType primitive_type)
    : mesh_(mesh)
    , wireframe_(false)
    , primitive_type_(primitive_type)
    , name_()
    , receive_shadow_(true)
{
}

const Mesh *RenderEntity::mesh() const
{
    return mesh_;
}

bool RenderEntity::should_render_wireframe() const
{
    return wireframe_;
}

void RenderEntity::set_wireframe(const bool wireframe)
{
    wireframe_ = wireframe;
}

PrimitiveType RenderEntity::primitive_type() const
{
    return primitive_type_;
}

std::string RenderEntity::name() const
{
    return name_;
}

void RenderEntity::set_name(std::string_view name)
{
    name_ = name;
}

bool RenderEntity::receive_shadow() const
{
    return receive_shadow_;
}

void RenderEntity::set_receive_shadow(bool receive_shadow)
{
    receive_shadow_ = receive_shadow;
}

}
