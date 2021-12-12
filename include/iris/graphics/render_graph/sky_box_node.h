////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graphics/cube_map.h"
#include "graphics/render_graph/render_node.h"

namespace iris
{

class ShaderCompiler;

class SkyBoxNode : public RenderNode
{
  public:
    /**
     * Implementation of RenderNode which renders a sky box.
     *
     * @param sky_box
     *   CubeMap to use for sky box.
     */
    SkyBoxNode(const CubeMap *sky_box);

    ~SkyBoxNode() override = default;

    /**
     * Accept a compiler visitor.
     *
     * @param compiler
     *   Compiler to accept.
     */
    void accept(ShaderCompiler &compiler) const override;

    /**
     * Get sky box.
     *
     * @returns
     *   Cube map for sky box.
     */
    const CubeMap *sky_box() const;

  private:
    /** Cube map for sky box. */
    const CubeMap *sky_box_;
};

}
