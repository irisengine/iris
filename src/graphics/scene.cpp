////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/scene.h"

#include <vector>

#include "core/colour.h"
#include "core/error_handling.h"
#include "graphics/lights/lighting_rig.h"
#include "graphics/render_entity.h"
#include "graphics/render_graph/render_graph.h"

namespace iris
{
Scene::Scene(RenderGraph *default_render_graph, bool *dirty_pipeline)
    : entities_()
    , lighting_rig_()
    , default_render_graph_(default_render_graph)
    , dirty_pipeline_(dirty_pipeline)
{
    lighting_rig_.ambient_light = std::make_unique<AmbientLight>(Colour{1.0f, 1.0f, 1.0f});
}

RenderEntity *Scene::add(RenderGraph *render_graph, std::unique_ptr<RenderEntity> entity)
{
    *dirty_pipeline_ = true;

    if (render_graph == nullptr)
    {
        render_graph = default_render_graph_;
        entity->set_receive_shadow(false);
    }

    entities_.emplace_back(render_graph, std::move(entity));

    return std::get<1>(entities_.back()).get();
}

void Scene::remove(RenderEntity *entity)
{
    *dirty_pipeline_ = true;

    entities_.erase(
        std::remove_if(
            std::begin(entities_),
            std::end(entities_),
            [entity](const auto &e) { return std::get<1>(e).get() == entity; }),
        std::end(entities_));
}

PointLight *Scene::add(std::unique_ptr<PointLight> light)
{
    *dirty_pipeline_ = true;

    lighting_rig_.point_lights.emplace_back(std::move(light));
    return lighting_rig_.point_lights.back().get();
}

DirectionalLight *Scene::add(std::unique_ptr<DirectionalLight> light)
{
    *dirty_pipeline_ = true;

    lighting_rig_.directional_lights.emplace_back(std::move(light));
    return lighting_rig_.directional_lights.back().get();
}

Colour Scene::ambient_light() const
{
    return lighting_rig_.ambient_light->colour();
}

void Scene::set_ambient_light(const Colour &colour)
{
    *dirty_pipeline_ = true;

    lighting_rig_.ambient_light->set_colour(colour);
}

RenderGraph *Scene::render_graph(RenderEntity *entity) const
{
    auto found = std::find_if(std::cbegin(entities_), std::cend(entities_), [entity](const auto &element) {
        return std::get<1>(element).get() == entity;
    });

    expect(found == std::cend(entities_), "entity not in scene");

    return std::get<0>(*found);
}

std::vector<std::tuple<RenderGraph *, std::unique_ptr<RenderEntity>>> &Scene::entities()
{
    return entities_;
}

const std::vector<std::tuple<RenderGraph *, std::unique_ptr<RenderEntity>>> &Scene::entities() const
{
    return entities_;
}

const LightingRig *Scene::lighting_rig() const
{
    return &lighting_rig_;
}

}
