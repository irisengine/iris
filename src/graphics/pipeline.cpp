#include "graphics/pipeline.h"

#include <memory>
#include <tuple>
#include <vector>

#include "core/camera.h"
#include "core/matrix4.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/render_target.h"
#include "graphics/stage.h"

namespace iris
{

void Pipeline::add_stage(std::unique_ptr<Scene> scene, Camera &camera)
{
    add_stage(std::move(scene), camera, nullptr);
}

void Pipeline::add_stage(
    std::unique_ptr<Scene> scene,
    Camera &camera,
    RenderTarget *target)
{
    scenes_.emplace_back(std::move(scene));

    auto *current_scene = scenes_.back().get();

    // for each light (that casts shadows) we need to add a new stage to the
    // pipeline to create a shadow map
    // note that this us done *before* we add the stage for the scene passed
    // in as we will need all the shadow map stages to be executed before we can
    // render that scene
    for (const auto &light :
         current_scene->lighting_rig()->directional_lights)
    {
        if (light->casts_shadows())
        {
            // for the shadow map stage we only want to render depth
            // information, so we set depth only for all scene entities
            // this will let the shader compiler generate more efficient shaders
            //
            // note that we will need to revert this before we add the passed in
            // scene
            for (auto &[graph, entity] : current_scene->entities())
            {
                graph->render_node()->set_depth_only(true);
            }

            // add a shadow map stage
            stages_.emplace_back(std::make_unique<Stage>(
                current_scene, light->shadow_camera(), light->shadow_target()));

            for (auto &[graph, entity] : current_scene->entities())
            {
                // revert depth only mode
                graph->render_node()->set_depth_only(false);

                // if the entity can receive shadows then we need to wire the
                // shadow map into into its render graph
                if (entity->receive_shadow())
                {
                    auto *texture = graph->create<TextureNode>(
                        light->shadow_target()->depth_texture());
                    graph->render_node()->add_shadow_map_input(texture);
                }
            }
        }
    }

    // add the requested scene
    stages_.emplace_back(
        std::make_unique<Stage>(current_scene, camera, target));
}

const std::vector<std::unique_ptr<Stage>> &Pipeline::stages() const
{
    return stages_;
}
}
