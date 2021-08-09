#include "graphics/render_queue_builder.h"

#include <map>
#include <vector>

#include "graphics/render_graph/render_graph.h"
#include "graphics/render_target.h"
#include "graphics/renderer.h"

namespace
{

/**
 * Helper function to create and enqueue all commands for rendering a Scene with
 * a given light type.
 *
 * @param scene
 *   Scene to render.
 *
 * @param light_type
 *   Type of light for the scene.
 *
 * @param cmd
 *   Command object to mutate and enqueue, this is passed in so it can be
 *   "pre-loaded" with the correct state.
 *
 * @param create_material_callback
 *   Callback for creating a Material object.
 *
 * @param render_queue
 *   Queue to add render commands to.
 *
 * @param shadow_maps
 *   Map of directional lights to their associated shadow map render target.
 */
void encode_light_pass_commands(
    const iris::Scene *scene,
    iris::LightType light_type,
    iris::RenderCommand &cmd,
    iris::RenderQueueBuilder::CreateMaterialCallback create_material_callback,
    std::vector<iris::RenderCommand> &render_queue,
    const std::map<iris::DirectionalLight *, iris::RenderTarget *> &shadow_maps)
{
    // create commands for each entity in the scene
    for (const auto &[render_graph, render_entity] : scene->entities())
    {
        auto *material = create_material_callback(
            render_graph, render_entity.get(), light_type);
        cmd.set_material(material);

        // renderer implementation will handle duplicate checking
        cmd.set_type(iris::RenderCommandType::UPLOAD_TEXTURE);
        render_queue.push_back(cmd);

        cmd.set_render_entity(render_entity.get());

        // light specific draw commands
        switch (light_type)
        {
            case iris::LightType::AMBIENT:
                cmd.set_type(iris::RenderCommandType::DRAW);
                cmd.set_light(scene->lighting_rig()->ambient_light.get());
                render_queue.push_back(cmd);
                break;
            case iris::LightType::POINT:
                // a draw command for each light
                for (auto &light : scene->lighting_rig()->point_lights)
                {
                    cmd.set_type(iris::RenderCommandType::DRAW);
                    cmd.set_light(light.get());
                    render_queue.push_back(cmd);
                }
                break;
            case iris::LightType::DIRECTIONAL:
                // a draw command for each light
                for (auto &light : scene->lighting_rig()->directional_lights)
                {
                    cmd.set_type(iris::RenderCommandType::DRAW);
                    cmd.set_light(light.get());

                    // set shadow map in render command
                    if (render_entity->receive_shadow())
                    {
                        auto *shadow_map = shadow_maps.count(light.get()) == 0u
                                               ? nullptr
                                               : shadow_maps.at(light.get());
                        cmd.set_shadow_map(shadow_map);
                    }

                    render_queue.push_back(cmd);
                }
                break;
        }
    }
}

}

namespace iris
{

RenderQueueBuilder::RenderQueueBuilder(
    CreateMaterialCallback create_material_callback,
    CreateRenderTargetCallback create_render_target_callback)
    : create_material_callback_(create_material_callback)
    , create_render_target_callback_(create_render_target_callback)
{
}

std::vector<RenderCommand> RenderQueueBuilder::build(
    std::vector<RenderPass> &render_passes) const
{
    std::map<DirectionalLight *, RenderTarget *> shadow_maps;
    std::vector<RenderPass> shadow_passes{};

    // for each shadow casting light create a render target for the shadow map
    // and enqueue commands so they are rendered
    for (const auto &pass : render_passes)
    {
        for (const auto &light : pass.scene->lighting_rig()->directional_lights)
        {
            if (light->casts_shadows())
            {
                auto *rt = create_render_target_callback_(1024u, 1024u);
                RenderPass shadow_pass = pass;
                shadow_pass.camera = std::addressof(light->shadow_camera());
                shadow_pass.render_target = rt;
                shadow_pass.depth_only = true;

                shadow_passes.emplace_back(shadow_pass);

                shadow_maps[light.get()] = rt;
            }
        }
    }

    // insert shadow passes into the queue
    render_passes.insert(
        std::cbegin(render_passes),
        std::cbegin(shadow_passes),
        std::cend(shadow_passes));

    std::vector<RenderCommand> render_queue;

    RenderCommand cmd{};

    // convert each pass into a series of commands which will render it
    for (auto &pass : render_passes)
    {
        const auto has_directional_light_pass =
            !pass.depth_only &&
            !pass.scene->lighting_rig()->directional_lights.empty();
        const auto has_point_light_pass =
            !pass.depth_only &&
            !pass.scene->lighting_rig()->point_lights.empty();

        cmd.set_render_pass(std::addressof(pass));

        cmd.set_type(RenderCommandType::PASS_START);
        render_queue.push_back(cmd);

        // always encode ambient light pass
        encode_light_pass_commands(
            pass.scene,
            LightType::AMBIENT,
            cmd,
            create_material_callback_,
            render_queue,
            shadow_maps);

        // encode point lights if there are any
        if (has_point_light_pass)
        {
            encode_light_pass_commands(
                pass.scene,
                LightType::POINT,
                cmd,
                create_material_callback_,
                render_queue,
                shadow_maps);
        }

        // encode directional lights if there are any
        if (has_directional_light_pass)
        {
            encode_light_pass_commands(
                pass.scene,
                LightType::DIRECTIONAL,
                cmd,
                create_material_callback_,
                render_queue,
                shadow_maps);
        }

        cmd.set_type(RenderCommandType::PASS_END);
        render_queue.push_back(cmd);
    }

    cmd.set_type(RenderCommandType::PRESENT);
    render_queue.push_back(cmd);

    return render_queue;
}

}
