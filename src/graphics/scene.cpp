#include "graphics/scene.h"

#include <vector>

#include "graphics/lights/lighting_rig.h"
#include "graphics/material.h"
#include "graphics/render_entity.h"
#include "graphics/render_graph/render_graph.h"

namespace iris
{
Scene::Scene()
    : entities_()
    , lighting_rig_()
{
    set_ambient_light({1.0f, 1.0f, 1.0f});
}

RenderEntity *Scene::add(
    RenderGraph *render_graph,
    std::unique_ptr<RenderEntity> entity)
{
    static RenderGraph default_graph{};
    auto *rg = render_graph == nullptr ? &default_graph : render_graph;

    entities_.emplace_back(rg, std::move(entity));

    return std::get<1>(entities_.back()).get();
}

PointLight *Scene::add(std::unique_ptr<PointLight> light)
{
    lighting_rig_.point_lights.emplace_back(std::move(light));
    return lighting_rig_.point_lights.back().get();
}

DirectionalLight *Scene::add(std::unique_ptr<DirectionalLight> light)
{
    lighting_rig_.directional_lights.emplace_back(std::move(light));
    return lighting_rig_.directional_lights.back().get();
}

Colour Scene::ambient_light() const
{
    return lighting_rig_.ambient_light;
}

void Scene::set_ambient_light(const Colour &colour)
{
    lighting_rig_.ambient_light = colour;
}

std::vector<std::tuple<RenderGraph *, std::unique_ptr<RenderEntity>>>
    &Scene::entities()
{
    return entities_;
}

const LightingRig *Scene::lighting_rig()
{
    return &lighting_rig_;
}

}
