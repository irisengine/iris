#include "graphics/stage.h"

#include <memory>
#include <tuple>
#include <vector>

#include "core/camera.h"
#include "core/root.h"
#include "graphics/material.h"
#include "graphics/render_entity.h"
#include "graphics/render_target.h"
#include "graphics/scene.h"

namespace iris
{

Stage::Stage(std::unique_ptr<Scene> scene, Camera &camera)
    : Stage(std::move(scene), camera, Root::screen_target())
{
}

Stage::Stage(std::unique_ptr<Scene> scene, Camera &camera, RenderTarget &target)
    : scene_(std::move(scene))
    , camera_(camera)
    , target_(target)
{
    for (const auto &[render_graph, entity] : scene_->entities())
    {
        auto material = std::make_unique<Material>(
            render_graph, entity->buffer_descriptors().front());

        materials_.emplace_back(std::move(material));
        render_items_.emplace_back(entity.get(), materials_.back().get());
    }
}

std::vector<std::tuple<RenderEntity *, Material *>> Stage::render_items() const
{
    return render_items_;
}

Camera &Stage::camera()
{
    return camera_;
}

RenderTarget &Stage::target()
{
    return target_;
}

}
