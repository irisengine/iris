#include "graphics/stage.h"

#include <map>
#include <memory>
#include <tuple>
#include <vector>

#include "core/camera.h"
#include "graphics/lights/lighting_rig.h"
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
    rebuild(scene_);
}

void Stage::rebuild(Scene *scene)
{
    render_items_.clear();
    materials_.clear();

    // simple cache to avoid create duplicate materials
    std::map<RenderGraph *, Material *> material_cache{};

    // create a RenderItem for each entity in the scene
    for (const auto &[render_graph, entity] : scene->entities())
    {
        Material *material = nullptr;
        const auto cach_entry = material_cache.find(render_graph);

        // use a simple cache to avoid compiling and creating duplicate
        // materials
        if (cach_entry == std::cend(material_cache))
        {
            auto new_material = std::make_unique<Material>(
                render_graph, entity->meshes().front(), scene->lighting_rig());

            materials_.emplace_back(std::move(new_material));
            material = materials_.back().get();
            material_cache[render_graph] = material;
        }
        else
        {
            material = cach_entry->second;
        }

        render_items_.emplace_back(
            RenderItem{entity.get(), material, scene->lighting_rig()});
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
