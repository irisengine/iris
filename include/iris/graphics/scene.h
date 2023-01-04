////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include "graphics/instanced_entity.h"
#include "graphics/lights/lighting_rig.h"
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
    template <class T, class... Args>
    T *create_entity(RenderGraph *render_graph, Args &&...args)
    {
        auto element = std::make_unique<T>(std::forward<Args>(args)...);

        return static_cast<T *>(add(std::move(render_graph), std::move(element)));
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
    RenderEntity *add(RenderGraph *render_graph, std::unique_ptr<RenderEntity> entity);

    /**
     * Remove an entity from the scene.
     *
     * @param entity
     *   The entity to remove.
     */
    void remove(RenderEntity *entity);

    /**
     * Create a Light and add it to the scene. Uses perfect forwarding to pass
     * along all arguments.
     *
     * @param args
     *   Args for light.
     *
     * @returns
     *   Pointer to newly created light.
     */
    template <class T, class... Args>
    T *create_light(Args &&...args)
    {
        auto light = std::make_unique<T>(std::forward<Args>(args)...);
        return add(std::move(light));
    }

    /**
     * Add a point light to the scene.
     *
     * @param light
     *   Light to add to scene
     *
     * @returns
     *   Pointer to the added light.
     */
    PointLight *add(std::unique_ptr<PointLight> light);

    /**
     * Add a directional light to the scene.
     *
     * @param light
     *   Light to add to scene
     *
     * @returns
     *   Pointer to the added light.
     */
    DirectionalLight *add(std::unique_ptr<DirectionalLight> light);

    /**
     * Get ambient light colour.
     *
     * @returns
     *   Ambient light colour.
     */
    Colour ambient_light() const;

    /**
     * Set ambient light colour.
     *
     * @param colour
     *   New ambient light colour.
     */
    void set_ambient_light(const Colour &colour);

    /**
     * Get the RenderGraph for a given RenderEntity.
     *
     * @param entity
     *   RenderEntity to get RenderGraph for.
     *
     * @returns
     *   RenderGraph for supplied RenderEntity.
     */
    RenderGraph *render_graph(RenderEntity *entity) const;

    /**
     * Get a reference to all entities in the scene.
     *
     * @returns
     *   Collection of <RenderGraph, RenderEntity> tuples.
     */
    std::vector<std::tuple<RenderGraph *, std::unique_ptr<RenderEntity>>> &entities();

    /**
     * Get a const reference to all entities in the scene.
     *
     * @returns
     *   Collection of <RenderGraph, RenderEntity> tuples.
     */
    const std::vector<std::tuple<RenderGraph *, std::unique_ptr<RenderEntity>>> &entities() const;

    /**
     * Get LightingRig.
     *
     * @returns
     *   Pointer to LightingRig.
     */
    const LightingRig *lighting_rig() const;

  private:
    // friend to allow only the RenderPipeline to create
    friend class RenderPipeline;

    /**
     * Create new Scene.
     *
     * @param default_render_graph
     *   Render graph to use when a user does to supply one.
     *
     * @param dirty_pipeline
     *   Flag to set when the scene is modified.
     */
    Scene(RenderGraph *default_render_graph, bool *dirty_pipeline);

    /**
     * Create a RenderEntity and add it to the scene. Uses perfect forwarding to pass along all arguments.
     *
     * Note that this inserts into the internal collection of entities at the front. It's a private method so only
     * RenderPipeline can call it, this is because:
     *  1. A user shouldn't have to care about internal entity ordering
     *  2. RenderPipeline does care about ordering and needs a bit more control.
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
    template <class T, class... Args>
    T *create_entity_at_front(RenderGraph *render_graph, Args &&...args)
    {
        auto element = std::make_unique<T>(std::forward<Args>(args)...);

        return static_cast<T *>(add_at_front(std::move(render_graph), std::move(element)));
    }

    /**
     * Add a RenderEntity to the scene.
     *
     * Note that this inserts into the internal collection of entities at the front. It's a private method so only
     * RenderPipeline can call it, this is because:
     *  1. A user shouldn't have to care about internal entity ordering
     *  2. RenderPipeline does care about ordering and needs a bit more control.
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
    RenderEntity *add_at_front(RenderGraph *render_graph, std::unique_ptr<RenderEntity> entity);

    /** Collection of <RenderGraph, RenderEntity> tuples. */
    std::vector<std::tuple<RenderGraph *, std::unique_ptr<RenderEntity>>> entities_;

    /** Collection of RenderGraphs. */
    std::vector<std::unique_ptr<RenderGraph>> render_graphs_;

    /** Lighting rig for scene. */
    LightingRig lighting_rig_;

    /** Handle to the default render graph. */
    RenderGraph *default_render_graph_;

    /** Flag to indicate user has changed the scene. */
    bool *dirty_pipeline_;
};

}
