////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>

#include <core/exception.h>
#include <core/looper.h>
#include <core/resource_loader.h>
#include <core/root.h>
#include <core/start.h>
#include <graphics/mesh_manager.h>
#include <graphics/post_processing_description.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_graph/texture_node.h>
#include <graphics/render_pipeline.h>
#include <graphics/render_target.h>
#include <graphics/render_target_manager.h>
#include <graphics/scene.h>
#include <graphics/single_entity.h>
#include <graphics/text_factory.h>
#include <graphics/texture_manager.h>
#include <graphics/window.h>
#include <graphics/window_manager.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "samples/animation_sample.h"
#include "samples/physics_sample.h"
#include "samples/render_graph_sample.h"
#include "samples/sample.h"

static constexpr std::size_t sample_count = 3u;

using namespace std::chrono_literals;

struct RenderState
{
    std::unique_ptr<iris::RenderPipeline> render_pipeline;
    std::unique_ptr<Sample> sample;
};

std::unique_ptr<Sample> create_sample(iris::Window *window, iris::RenderPipeline &render_pipeline, std::size_t index)
{
    switch (index)
    {
        case 0: return std::make_unique<AnimationSample>(window, render_pipeline); break;
        case 1: return std::make_unique<RenderGraphSample>(window, render_pipeline); break;
        case 2: return std::make_unique<PhysicsSample>(window, render_pipeline); break;
        default: throw iris::Exception("unknown sample index");
    }
}

RenderState create_render_state(iris::Window *window, iris::Camera *camera, std::size_t index)
{
    auto render_pipeline = std::make_unique<iris::RenderPipeline>(window->width(), window->height());
    auto sample = create_sample(window, *render_pipeline, index % sample_count);

    const auto *sample_target = sample->target();

    auto *scene = render_pipeline->create_scene();
    auto *rg = render_pipeline->create_render_graph();
    rg->render_node()->set_colour_input(rg->create<iris::TextureNode>(sample_target->colour_texture()));
    scene->create_entity<iris::SingleEntity>(
        rg, iris::Root::mesh_manager().sprite({1.0f, 1.0f, 1.0f}), iris::Transform{{0.0f}, {}, {800.0f, 800.0f, 1.0f}});

    iris::PostProcessingDescription post_processing_description{
        .bloom = {iris::BloomDescription{6.0f}},
        .colour_adjust = {iris::ColourAdjustDescription{}},
        .anti_aliasing = {iris::AntiAliasingDescription{}}};

    auto *pass = render_pipeline->create_render_pass(scene);
    pass->camera = camera;
    pass->post_processing_description = post_processing_description;

    return {std::move(render_pipeline), std::move(sample)};
}

void go(int, char **)
{
    iris::ResourceLoader::instance().set_root_directory("assets");
    auto &rtm = iris::Root::render_target_manager();

    auto window = iris::Root::window_manager().create_window(800u, 800u);
    std::size_t sample_number = 0u;

    iris::Camera camera{iris::CameraType::ORTHOGRAPHIC, window->width(), window->height()};

    auto [render_pipeline, sample] = create_render_state(window, &camera, sample_number);

    window->set_render_pipeline(std::move(render_pipeline));

    std::size_t frame_counter = 0u;
    std::size_t next_update = 1u;

    iris::Looper looper{
        0ms,
        16ms,
        [&sample = sample](auto, auto) {
            sample->fixed_update();
            return true;
        },
        [&, &sample = sample](std::chrono::microseconds elapsed, auto) {
            auto running = true;
            auto event = window->pump_event();
            while (event)
            {
                if (event->is_quit() || event->is_key(iris::Key::ESCAPE))
                {
                    running = false;
                }
                else if (event->is_key(iris::Key::TAB, iris::KeyState::UP))
                {
                    ++sample_number;

                    auto [new_render_pipeline, new_sample] = create_render_state(window, &camera, sample_number);
                    sample = std::move(new_sample);
                    window->set_render_pipeline(std::move(new_render_pipeline));
                }
                else
                {
                    sample->handle_input(*event);
                }
                event = window->pump_event();
            }

            sample->variable_update();
            window->render();

            return running;
        }};

    looper.run();

    iris::Root::reset();
}

int main(int argc, char **argv)
{
    iris::start_debug(argc, argv, go);
}