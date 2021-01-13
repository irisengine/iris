#include <iostream>

#include "core/camera.h"
#include "core/quaternion.h"

#include "graphics/mesh_factory.h"
#include "graphics/pipeline.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/render_graph/texture_node.h"
#include "graphics/render_system.h"
#include "graphics/scene.h"
#include "graphics/stage.h"
#include "graphics/texture.h"
#include "graphics/texture_factory.h"
#include "log/log.h"
#include "platform/event.h"
#include "platform/start.h"
#include "platform/window.h"

void go(int, char **)
{
    LOG_DEBUG("sprite_sample", "hello world");

    iris::Window window{800.0f, 800.0f};

    iris::Camera screen_camera{iris::CameraType::ORTHOGRAPHIC, 800.0f, 800.0f};

    auto scene = std::make_unique<iris::Scene>();

    auto *sprite1 = scene->create_entity(
        iris::RenderGraph{},
        iris::mesh_factory::sprite({0.39f, 0.58f, 0.92f}),
        iris::Vector3{0.0f},
        iris::Vector3{100.0f});

    auto *sprite2 = scene->create_entity(
        iris::RenderGraph{},
        iris::mesh_factory::sprite({0.86f, 0.08f, 0.23f}),
        iris::Vector3{0.0f, 300.0f, 0.0f},
        iris::Vector3{100.0f});

    iris::RenderGraph graph{};
    auto *texture_node = graph.create<iris::TextureNode>("circle.png");
    graph.render_node()->set_colour_input(texture_node);

    auto *sprite3 = scene->create_entity(
        std::move(graph),
        iris::mesh_factory::sprite({1.0f}),
        iris::Vector3{0.0f, -300.0f, 0.0f},
        iris::Vector3{100.0f});

    iris::Quaternion rot{{0.0f, 0.0f, 1.0f}, 0.0f};
    iris::Quaternion delta{{0.0f, 0.0f, 1.0f}, 0.02f};

    iris::Pipeline pipeline{};
    pipeline.add_stage(std::move(scene), screen_camera);

    for (;;)
    {
        if (auto evt = window.pump_event(); evt)
        {
            if (evt->is_key(iris::Key::Q))
            {
                break;
            }
        }

        sprite1->set_orientation(rot);
        sprite2->set_orientation(rot);
        sprite3->set_orientation(rot);
        rot *= delta;

        window.render(pipeline);
    }
    LOG_ERROR("sprite_sample", "goodbye!");
}

int main(int argc, char **argv)
{
    iris::start_debug(argc, argv, go);
}
