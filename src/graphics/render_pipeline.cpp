////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/render_pipeline.h"

#include <cstddef>
#include <memory>
#include <vector>

#include "core/error_handling.h"
#include "core/root.h"
#include "graphics/material_manager.h"
#include "graphics/mesh_manager.h"
#include "graphics/render_graph/arithmetic_node.h"
#include "graphics/render_graph/blur_node.h"
#include "graphics/render_graph/component_node.h"
#include "graphics/render_graph/conditional_node.h"
#include "graphics/render_graph/node.h"
#include "graphics/render_graph/post_processing/ambient_occlusion_node.h"
#include "graphics/render_graph/post_processing/anti_aliasing_node.h"
#include "graphics/render_graph/post_processing/colour_adjust_node.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/render_graph/sky_box_node.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/render_graph/value_node.h"
#include "graphics/render_pass.h"
#include "graphics/render_pipeline.h"
#include "graphics/render_target.h"
#include "graphics/render_target_manager.h"
#include "graphics/renderer.h"
#include "graphics/scene.h"
#include "graphics/single_entity.h"
#include "log/log.h"

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
 * @param has_normal_target
 *   Flag indicating if normals should be rendered.
 *
 * @param has_position_target
 *   Flag indicating if positions should be rendered.
 *
 * @param sky_box_render_graph
 *   Render graph for the sky box for this scene (if one is present), otherwise nullptr.
 *
 * @param cmd
 *   Command object to mutate and enqueue, this is passed in so it can be "pre-loaded" with the correct state.
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
    bool has_normal_target,
    bool has_position_target,
    const iris::RenderGraph *sky_box_render_graph,
    iris::RenderCommand &cmd,
    std::vector<iris::RenderCommand> &render_queue,
    const std::unordered_map<iris::DirectionalLight *, iris::RenderTarget *> &shadow_maps)
{
    for (const auto &[render_graph, render_entity] : scene->entities())
    {
        // if we have a sky box we only want to render it once on the ambient pass
        if ((render_graph == sky_box_render_graph) && (light_type != iris::LightType::AMBIENT))
        {
            continue;
        }

        auto *material = iris::Root::material_manager().create(
            render_graph,
            render_entity.get(),
            light_type,
            cmd.render_pass()->colour_target != nullptr,
            has_normal_target && (light_type == iris::LightType::AMBIENT),
            has_position_target && (light_type == iris::LightType::AMBIENT),
            render_entity->has_transparency());
        cmd.set_material(material);

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
                        auto *shadow_map = shadow_maps.count(light.get()) == 0u ? nullptr : shadow_maps.at(light.get());
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

RenderPipeline::RenderPipeline(std::uint32_t width, std::uint32_t height)
    : scenes_()
    , render_graphs_()
    , user_created_passes_()
    , engine_created_passes_()
    , render_passes_()
    , sky_box_entities_()
    , dirty_(false)
    , width_(width)
    , height_(height)
    , cameras_()
    , shadow_maps_()
{
}

RenderPipeline::~RenderPipeline() = default;

Scene *RenderPipeline::create_scene()
{
    static RenderGraph default_render_graph{};

    // using new to access private ctor
    scenes_.push_back(std::unique_ptr<Scene>(new Scene{&default_render_graph, &dirty_}));

    return scenes_.back().get();
}

RenderGraph *RenderPipeline::create_render_graph()
{
    // using new to access private ctor
    render_graphs_.push_back(std::unique_ptr<RenderGraph>(new RenderGraph{}));

    return render_graphs_.back().get();
}

RenderPass *RenderPipeline::create_render_pass(Scene *scene)
{
    // using new to access private ctor
    auto pass = std::unique_ptr<RenderPass>(new RenderPass{});
    pass->scene = scene;

    user_created_passes_.push_back(std::move(pass));

    return user_created_passes_.back().get();
}

std::vector<RenderCommand> RenderPipeline::build()
{
    engine_created_passes_.clear();
    sky_box_entities_.clear();
    shadow_maps_.clear();
    std::vector<RenderPass *> pre_process_passes{};

    // for each shadow casting light create a render target for the shadow map
    // and enqueue commands so they are rendered
    for (const auto &pass : user_created_passes_)
    {
        for (const auto &light : pass->scene->lighting_rig()->directional_lights)
        {
            if (light->casts_shadows())
            {
                auto *rt = Root::render_target_manager().create(1024u, 1024u);
                RenderPass *shadow_pass = create_engine_render_pass(pass->scene);
                shadow_pass->post_processing_description = {};
                shadow_pass->camera = std::addressof(light->shadow_camera());
                shadow_pass->colour_target = rt;
                shadow_pass->normal_target = nullptr;
                shadow_pass->position_target = nullptr;
                shadow_pass->depth_only = true;
                shadow_pass->clear_colour = true;
                shadow_pass->clear_depth = true;

                pre_process_passes.push_back(shadow_pass);

                shadow_maps_[light.get()] = rt;
            }
        }

        // SSAO is a bit messy to integrate, we first need to create a pass to output all the required information
        // such as screen space normals and positions and then add the pass to combine all this data into our
        // occlusion texture later on we will then wire this into the normal lighting passes, due to how the render
        // is setup it's not easy to just add the occlusion texture into the ambient light pass, so instead we do
        // that whole calculation in the SSAO pass and use that instead of recalculating the ambient pass
        if (const auto ssao = pass->post_processing_description.ambient_occlusion; ssao)
        {
            // add a pass to output the data we need
            RenderPass *ao_data_pass = create_engine_render_pass(pass->scene);
            ao_data_pass->post_processing_description = {};
            ao_data_pass->colour_target = nullptr;
            ao_data_pass->normal_target = Root::render_target_manager().create(width_, height_);
            ao_data_pass->position_target = Root::render_target_manager().create(width_, height_);
            ao_data_pass->depth_only = true;
            ao_data_pass->clear_colour = true;
            ao_data_pass->camera = pass->camera;

            pre_process_passes.push_back(ao_data_pass);
            auto *prev = pre_process_passes.back();

            const auto prev_camera = prev->camera;

            // add a pass to calculate ssao (combined with the ambient light pass)
            auto *ao_target = add_pass(pre_process_passes, [prev, ssao](RenderGraph *rg, const RenderTarget *target) {
                rg->set_render_node<AmbientOcclusionNode>(
                    rg->create<TextureNode>(target->colour_texture()),
                    rg->create<TextureNode>(prev->normal_target->colour_texture()),
                    rg->create<TextureNode>(prev->position_target->colour_texture()),
                    *ssao);
            });

            // ensure we render with the perspective camera not the orthographic camera that will be created for the
            // new pass
            cameras_.back() = *prev_camera;

            prev = pre_process_passes.back();
            prev->colour_target = pass->colour_target;

            // fudge the colour target of the pre pass and the depth target of the ssao pass into one render target
            pass->colour_target = Root::render_target_manager().create(prev->colour_target, ao_target);
            pass->clear_colour = false;
            pass->clear_depth = false;
        }

        // if a skybox has been added ensure it has an entity in the scene
        if (pass->sky_box != nullptr)
        {
            auto *scene = pass->scene;

            auto *sky_box_rg = create_render_graph();
            sky_box_rg->set_render_node<SkyBoxNode>(pass->sky_box);

            const auto [_, inserted] = sky_box_entities_.try_emplace(
                pass.get(),
                scene->create_entity_at_front<SingleEntity>(
                    sky_box_rg, Root::mesh_manager().cube({}), Transform({}, {}, {0.5f})));
            expect(inserted, "sky box exists");

            sky_box_entities_[pass.get()]->set_receive_shadow(false);
            sky_box_render_graphs_[pass.get()] = sky_box_rg;
        }
    }

    render_passes_ = pre_process_passes;
    std::transform(
        std::begin(user_created_passes_),
        std::end(user_created_passes_),
        std::back_insert_iterator(render_passes_),
        [](const auto &element) { return element.get(); });

    add_post_processing_passes();

    return rebuild();
}

std::vector<RenderCommand> RenderPipeline::rebuild()
{
    std::vector<RenderCommand> render_queue;
    RenderCommand cmd{};

    // convert each pass into a series of commands which will render it
    for (auto *pass : render_passes_)
    {
        cmd.set_render_pass(pass);

        cmd.set_type(RenderCommandType::PASS_START);
        render_queue.push_back(cmd);

        const auto has_normal_target = pass->normal_target != nullptr;
        const auto has_position_target = pass->position_target != nullptr;

        const auto *sky_box_rg = (sky_box_render_graphs_.contains(pass)) ? sky_box_render_graphs_[pass] : nullptr;

        // encode ambient light pass unless we have used ssao (in which case this gets done by the ssao pass itself)
        if (!pass->post_processing_description.ambient_occlusion)
        {
            encode_light_pass_commands(
                pass->scene,
                LightType::AMBIENT,
                has_normal_target,
                has_position_target,
                sky_box_rg,
                cmd,
                render_queue,
                shadow_maps_);
        }

        if (!pass->depth_only)
        {
            // encode point lights if there are any
            if (!pass->scene->lighting_rig()->point_lights.empty())
            {
                encode_light_pass_commands(
                    pass->scene,
                    LightType::POINT,
                    has_normal_target,
                    has_position_target,
                    sky_box_rg,
                    cmd,
                    render_queue,
                    shadow_maps_);
            }

            // encode directional lights if there are any
            if (!pass->scene->lighting_rig()->directional_lights.empty())
            {
                encode_light_pass_commands(
                    pass->scene,
                    LightType::DIRECTIONAL,
                    has_normal_target,
                    has_position_target,
                    sky_box_rg,
                    cmd,
                    render_queue,
                    shadow_maps_);
            }
        }

        cmd.set_type(RenderCommandType::PASS_END);
        render_queue.push_back(cmd);
    }

    cmd.set_type(RenderCommandType::PRESENT);
    render_queue.push_back(cmd);

    return render_queue;
}

std::vector<RenderPass *> RenderPipeline::render_passes() const
{
    return render_passes_;
}

bool RenderPipeline::is_dirty() const
{
    return dirty_;
}

void RenderPipeline::clear_dirty_bit()
{
    dirty_ = false;
}

RenderPass *RenderPipeline::create_engine_render_pass(Scene *scene)
{
    // using new to access private ctor
    auto pass = std::unique_ptr<RenderPass>(new RenderPass{});
    pass->scene = scene;

    engine_created_passes_.push_back(std::move(pass));

    return engine_created_passes_.back().get();
}

const RenderTarget *RenderPipeline::add_pass(
    std::vector<RenderPass *> &render_passes,
    std::function<void(RenderGraph *, const RenderTarget *)> create_render_graph_callback)
{
    // create new pass with
    cameras_.push_back({CameraType::ORTHOGRAPHIC, width_, height_});
    auto *camera = std::addressof(cameras_.back());
    const auto *target = Root::render_target_manager().create(width_, height_);

    auto *scene = create_scene();
    auto *rg = create_render_graph();
    create_render_graph_callback(rg, target);
    scene->create_entity<SingleEntity>(
        rg,
        Root::mesh_manager().sprite({}),
        Transform({}, {}, {static_cast<float>(width_), static_cast<float>(height_), 1. - 1}));

    auto *pass = create_engine_render_pass(scene);
    pass->camera = camera;

    // wire up this pass
    render_passes.back()->colour_target = target;
    render_passes.push_back(pass);

    return target;
}

void RenderPipeline::add_post_processing_passes()
{
    // our class member render_passes_ contains all the user passes and *some* engine passes
    // in order to add the rest we copy them one by one to a new collection and, for each one, add any additioanl
    // post processing passes
    // at the end we copy that back to the class member render_passes_
    std::vector<RenderPass *> render_passes{};

    for (auto *render_pass : render_passes_)
    {
        auto &last_pass = render_passes.emplace_back(render_pass);
        const auto *input_target = last_pass->colour_target;

        const auto description = render_pass->post_processing_description;

        if (const auto bloom = description.bloom; bloom)
        {
            const auto *null_target = add_pass(render_passes, [](RenderGraph *rg, const RenderTarget *target) {
                rg->render_node()->set_colour_input(rg->create<TextureNode>(target->colour_texture()));
            });

            add_pass(render_passes, [&bloom](RenderGraph *rg, const RenderTarget *target) {
                rg->render_node()->set_colour_input(rg->create<ConditionalNode>(
                    rg->create<ArithmeticNode>(
                        rg->create<TextureNode>(target->colour_texture()),
                        rg->create<ValueNode<Colour>>(Colour{0.2126f, 0.7152f, 0.0722f, 0.0f}),
                        ArithmeticOperator::DOT),
                    rg->create<ValueNode<float>>(bloom->threshold),
                    rg->create<TextureNode>(target->colour_texture()),
                    rg->create<ValueNode<Colour>>(Colour{0.0f, 0.0f, 0.0f, 1.0f}),
                    ConditionalOperator::GREATER));
            });

            for (auto i = 0u; i < bloom->iterations; ++i)
            {
                add_pass(render_passes, [](RenderGraph *rg, const RenderTarget *target) {
                    rg->render_node()->set_colour_input(
                        rg->create<BlurNode>(rg->create<TextureNode>(target->colour_texture())));
                });
            }

            add_pass(render_passes, [null_target](RenderGraph *rg, const RenderTarget *target) {
                rg->render_node()->set_colour_input(rg->create<ArithmeticNode>(
                    rg->create<TextureNode>(null_target->colour_texture()),
                    rg->create<TextureNode>(target->colour_texture()),
                    ArithmeticOperator::ADD));
            });
        }

        if (const auto colour_adjust = description.colour_adjust; colour_adjust)
        {
            add_pass(render_passes, [&colour_adjust](RenderGraph *rg, const RenderTarget *target) {
                rg->set_render_node<ColourAdjustNode>(
                    rg->create<TextureNode>(target->colour_texture()), *colour_adjust);
            });
        }

        if (description.anti_aliasing)
        {
            add_pass(render_passes, [](RenderGraph *rg, const RenderTarget *target) {
                rg->set_render_node<AntiAliasingNode>(rg->create<TextureNode>(target->colour_texture()));
            });
        }

        render_passes.back()->colour_target = input_target;
    }

    render_passes_ = render_passes;
}

Scene *RenderPipeline::scene(std::size_t index) const
{
    ensure(index < scenes_.size(), "index out of bounds");
    return scenes_[index].get();
}

}
