#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>

#include <core/exception.h>
#include <core/looper.h>
#include <core/resource_loader.h>
#include <core/start.h>
#include <core/window.h>
#include <graphics/mesh_factory.h>
#include <graphics/pipeline.h>
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_graph/texture_node.h>
#include <graphics/render_target.h>
#include <graphics/scene.h>
#include <graphics/text_factory.h>
#include <graphics/texture_manager.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "samples/animation_sample.h"
#include "samples/physics_sample.h"
#include "samples/render_graph_sample.h"
#include "samples/sample.h"

static constexpr std::size_t sample_count = 3u;

using namespace std::chrono_literals;

std::unique_ptr<Sample> create_sample(
    iris::Window *window,
    iris::RenderTarget *screen_target,
    std::size_t index)
{
    std::unique_ptr<Sample> sample{};

    switch (index)
    {
        case 0:
            sample = std::make_unique<AnimationSample>(window, screen_target);
            break;
        case 1:
            sample = std::make_unique<RenderGraphSample>(window, screen_target);
            break;
        case 2:
            sample = std::make_unique<PhysicsSample>(window, screen_target);
            break;
        default: throw iris::Exception("unknown sample index");
    }

    return sample;
}

iris::Pipeline create_pipeline(
    const iris::Window &window,
    iris::RenderTarget *screen_target,
    iris::Scene **new_scene,
    iris::Camera &camera)
{
    auto scene = std::make_unique<iris::Scene>();
    auto render_graph = scene->create_render_graph();

    auto *texture = screen_target->colour_texture();
    render_graph->render_node()->set_colour_input(
        render_graph->create<iris::TextureNode>(texture));

    auto sprite = iris::mesh_factory::sprite(iris::Colour{1.0f, 1.0f, 1.0f});

    scene->create_entity(
        render_graph,
        std::move(sprite),
        iris::Transform{
            iris::Vector3{},
            iris::Quaternion{},
            iris::Vector3{
                static_cast<float>(texture->width() / window.screen_scale()),
                static_cast<float>(texture->height() / window.screen_scale()),
                1.0f}});

    iris::Pipeline pipeline{};
    *new_scene = pipeline.add_stage(std::move(scene), camera);

    return pipeline;
}

void update_title(
    iris::Scene *scene,
    iris::RenderEntity **current_title,
    iris::Pipeline *pipeline,
    const std::string &title,
    const iris::Vector3 &position)
{
    iris::RenderGraph *render_graph = nullptr;

    if (*current_title != nullptr)
    {
        render_graph = scene->render_graph(*current_title);
        iris::TextureManager::unload(
            static_cast<iris::TextureNode *>(
                render_graph->render_node()->colour_input())
                ->texture());

        scene->remove(*current_title);
    }
    else
    {
        render_graph = scene->create_render_graph();
    }

    auto *texture = iris::text_factory::create(
        "Helvetica", 20u, title, iris::Colour{1.0f, 1.0f, 1.0f});

    render_graph->render_node()->set_colour_input(
        render_graph->create<iris::TextureNode>(texture));

    auto sprite = iris::mesh_factory::sprite(iris::Colour{1.0f, 1.0f, 1.0f});

    *current_title = scene->create_entity(
        render_graph,
        std::move(sprite),
        iris::Transform{
            position * iris::Window::screen_scale(),
            iris::Quaternion{},
            iris::Vector3{
                static_cast<float>(texture->width()),
                static_cast<float>(texture->height()),
                1.0f}});

    pipeline->rebuild_stage(scene);
}

void go(int, char **)
{
    iris::ResourceLoader::instance().set_root_directory("assets");

    iris::Window window{800u, 800u};
    std::size_t sample_number = 0u;

    iris::RenderTarget target{window.width(), window.height()};
    iris::Camera camera{
        iris::CameraType::ORTHOGRAPHIC, window.width(), window.height()};
    iris::RenderEntity *title = nullptr;
    iris::RenderEntity *fps = nullptr;

    iris::Scene *scene = nullptr;
    auto pipeline = create_pipeline(window, &target, &scene, camera);

    auto sample = create_sample(&window, &target, sample_number % sample_count);
    update_title(
        scene, &title, &pipeline, sample->title(), {0.0f, 375.0f, 1.0f});
    update_title(scene, &fps, &pipeline, "0 fps", {350.0f, 375.0f, 1.0f});

    std::size_t frame_counter = 0u;
    std::size_t next_update = 1u;

    iris::Looper looper{
        0ms,
        33ms,
        [&sample](auto, auto) {
            sample->fixed_update();
            return true;
        },
        [&sample,
         &window,
         &sample_number,
         &pipeline,
         &target,
         scene,
         &title,
         &fps,
         &frame_counter,
         &next_update](std::chrono::microseconds elapsed, auto) {
            auto running = true;
            if (auto event = window.pump_event(); event)
            {
                if (event->is_quit() || event->is_key(iris::Key::ESCAPE))
                {
                    running = false;
                }
                else if (event->is_key(iris::Key::TAB, iris::KeyState::UP))
                {
                    ++sample_number;
                    sample = create_sample(
                        &window, &target, sample_number % sample_count);

                    update_title(
                        scene,
                        &title,
                        &pipeline,
                        sample->title(),
                        {0.0f, 375.0f, 1.0f});
                }
                else
                {
                    sample->handle_input(*event);
                }
            }

            sample->variable_update();
            window.render(pipeline);

            ++frame_counter;

            const auto elapsed_s =
                std::chrono::duration_cast<std::chrono::seconds>(elapsed)
                    .count();
            if (elapsed_s > next_update)
            {
                const auto frames_per_second = frame_counter / elapsed_s;

                update_title(
                    scene,
                    &fps,
                    &pipeline,
                    std::to_string(frames_per_second) + " fps",
                    {350.0f, 375.0f, 1.0f});
                next_update = elapsed_s + 1;
            }

            return running;
        }};

    looper.run();
}

int main(int argc, char **argv)
{
    iris::start(argc, argv, go);
}