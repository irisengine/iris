#include "graphics/scene.h"

#include <vector>

#include "graphics/material.h"
#include "graphics/render_entity.h"
#include "graphics/render_graph/render_graph.h"

namespace iris
{

RenderEntity *Scene::add(
    RenderGraph render_graph,
    std::unique_ptr<RenderEntity> entity)
{
    entities_.emplace_back(std::move(render_graph), std::move(entity));

    return std::get<1>(entities_.back()).get();
}

std::vector<std::tuple<RenderGraph, std::unique_ptr<RenderEntity>>>
    &Scene::entities()
{
    return entities_;
}

}
