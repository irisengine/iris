////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <span>

namespace iris
{

class RenderGraph;

/**
 * Abstract class for a Material - a class which which encapsulates how to render a Mesh.
 *
 * This interface is deliberately limited, most of the functionality is provided by the implementations, which in turn
 * is only used internally by the engine.
 */
class Material
{
  public:
    /**
     * Construct a new Material.
     *
     * @param render_graph
     *   The render render_graph for this material.
     */
    Material(const RenderGraph *render_graph);
    virtual ~Material() = default;

    /**
     * Get the property buffer for this material.
     *
     * @returns
     *   Property buffer span.
     */
    std::span<std::byte> property_buffer() const;

  private:
    /** Property buffer span. */
    std::span<std::byte> property_buffer_;
};

}
