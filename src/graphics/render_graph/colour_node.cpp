////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_graph/colour_node.h"

#include <cstddef>

#include "core/colour.h"
#include "core/utils.h"
#include "graphics/render_graph/shader_compiler.h"

namespace iris
{

ColourNode::ColourNode(const Colour &colour)
    : colour_(colour)
{
}

void ColourNode::accept(ShaderCompiler &compiler) const
{
    compiler.visit(*this);
}

Colour ColourNode::colour() const
{
    return colour_;
}

std::size_t ColourNode::hash() const
{
    return combine_hash(colour_, "colour_node");
}

}
