#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include "graphics/light.h"
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
    RenderEntity *create_entity(RenderGraph render_graph, Args &&... args)
    {
        auto element =
            std::make_unique<RenderEntity>(std::forward<Args>(args)...);

        return add(std::move(render_graph), std::move(element));
    }

    /**
     * Create a Light and add it to the scene. Uses perfect forwarding to pass
     * along all arguments.
     *
     * @param args
     *   Arguments for Light.
     *
     * @returns
     *   Pointer to the newly created Light.
     */
    template <class... Args>
    Light *create_light(Args &&... args)
    {
        auto light = std::make_unique<Light>(std::forward<Args>(args)...);

        return add(std::move(light));
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
     * Add a Light to the scene.
     *
     * @param light
     *   Light to add to scene.
     *
     * @returns
     *   Pointer to added Light.
     */
    Light *add(std::unique_ptr<Light> light);

    /**
     * Get a reference to all entities in the scene.
     *
     * @returns
     *   Collection of <RenderGraph, RenderEntity> tuples.
     */
    std::vector<std::tuple<RenderGraph, std::unique_ptr<RenderEntity>>>
        &entities();

    /**
     * Get a const reference to all the lights in the scene.
     *
     * @returns
     *   const reference to light collection.
     */
    const std::vector<std::unique_ptr<Light>> &lights() const;

  private:
    /** Collection of <RenderGraph, RenderEntity> tuples. */
    std::vector<std::tuple<RenderGraph, std::unique_ptr<RenderEntity>>>
        entities_;

    /** Collection of lights. */
    std::vector<std::unique_ptr<Light>> lights_;
};

}
