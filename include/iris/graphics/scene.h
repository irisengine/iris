#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include "graphics/material.h"
#include "graphics/render_entity.h"
#include "graphics/render_graph/render_graph.h"

namespace iris
{

/**
 * A scene is a collection of entities to be rendered. It owns the memory of its
 * render entities.
 */
class Scene
{
  public:
    /**
     * Create a RenderEntity and add it to the scene. Uses perfect forwarding to
     * pass along all arguments.
     *
     * @param render_graph
     *   RenderGraph for RenderEntity.
     *
     * @param args
     *   Arguments for RenderEntity.
     *
     * @returns
     *   Pointer to the newly created RenderEntity.
     */
    template <class... Args>
    RenderEntity *create(RenderGraph render_graph, Args &&... args)
    {
        auto element =
            std::make_unique<RenderEntity>(std::forward<Args>(args)...);

        return add(std::move(render_graph), std::move(element));
    }

    /**
     * Add a RenderEntity to the scene.
     *
     * @param render_graph
     *   RenderGraph for RenderEntity.
     *
     * @param entity
     *   RenderEntity to add to scene.
     *
     * @returns
     *   Pointer to the added RenderEntity.
     */
    RenderEntity *add(
        RenderGraph render_graph,
        std::unique_ptr<RenderEntity> entity);

    /**
     * Get a reference to all entities in the scene.
     *
     * @returns
     *   Collection of <RenderGraph, RenderEntity> tuples.
     */
    std::vector<std::tuple<RenderGraph, std::unique_ptr<RenderEntity>>>
        &entities();

  private:
    /** Collection of <RenderGraph, RenderEntity> tuples. */
    std::vector<std::tuple<RenderGraph, std::unique_ptr<RenderEntity>>>
        entities_;
};

}
