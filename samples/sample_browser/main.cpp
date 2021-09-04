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
#include <graphics/render_graph/render_graph.h>
#include <graphics/render_graph/texture_node.h>
#include <graphics/render_target.h>
#include <graphics/scene.h>
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

void go(int, char **)
{
    iris::ResourceLoader::instance().set_root_directory("assets");

    auto window = iris::Root::window_manager().create_window(800u, 800u);
    std::size_t sample_number = 0u;

    auto *target = window->create_render_target();
    iris::Camera camera{
        iris::CameraType::ORTHOGRAPHIC, window->width(), window->height()};
    iris::RenderEntity *title = nullptr;
    iris::RenderEntity *fps = nullptr;

    auto scene = std::make_unique<iris::Scene>();
    auto *rg = scene->create_render_graph();
    rg->render_node()->set_colour_input(
        rg->create<iris::TextureNode>(target->colour_texture()));
    scene->create_entity(
        rg,
        iris::Root::mesh_manager().sprite({1.0f, 1.0f, 1.0f}),
        iris::Transform{{0.0f}, {}, {800.0f, 800.0f, 1.0f}});

    auto sample = create_sample(window, target, sample_number % sample_count);

    iris::RenderPass pass{scene.get(), &camera, nullptr};

    auto passes = sample->render_passes();
    passes.emplace_back(pass);

    window->set_render_passes(passes);

    std::size_t frame_counter = 0u;
    std::size_t next_update = 1u;

    iris::Looper looper{
        0ms,
        16ms,
        [&sample](auto, auto) {
            sample->fixed_update();
            return true;
        },
        [&sample,
         &window,
         &sample_number,
         &target,
         &title,
         &fps,
         &pass,
         &frame_counter,
         &next_update](std::chrono::microseconds elapsed, auto) {
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
                    sample = create_sample(
                        window, target, sample_number % sample_count);

                    auto passes = sample->render_passes();
                    passes.emplace_back(pass);

                    window->set_render_passes(passes);
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
}

int main(int argc, char **argv)
{
    iris::start(argc, argv, go);
}