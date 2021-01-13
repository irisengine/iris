#include "graphics/stage.h"

#include <memory>
#include <tuple>
#include <vector>

#include "core/camera.h"
#include "graphics/light.h"
#include "graphics/material.h"
#include "graphics/render_entity.h"
#include "graphics/render_target.h"
#include "graphics/scene.h"

namespace iris
{

Stage::Stage(Scene *scene, Camera &camera)
    : Stage(scene, camera, nullptr)
{
}

Stage::Stage(Scene *scene, Camera &camera, RenderTarget *target)
    : scene_(scene)
    , camera_(camera)
    , target_(target)
    , render_items_()
    , materials_()
{
    std::vector<Light *> lights{};

    for (const auto &light : scene_->lights())
    {
        lights.emplace_back(light.get());
    }

    for (const auto &[render_graph, entity] : scene_->entities())
    {
        auto material = std::make_unique<Material>(
            render_graph, entity->buffer_descriptors().front(), lights);

        materials_.emplace_back(std::move(material));
        render_items_.emplace_back(
            RenderItem{entity.get(), materials_.back().get(), lights});
    }
}

std::vector<RenderItem> Stage::render_items() const
{
    return render_items_;
}

Camera &Stage::camera()
{
    return camera_;
}

RenderTarget *Stage::target()
{
    return target_;
}

}
