#include <iostream>
#include <map>
#include <memory>

#include <iris/core/camera.h>
#include <iris/core/colour.h>
#include <iris/core/resource_loader.h>
#include <iris/core/root.h>
#include <iris/core/start.h>
#include <iris/core/transform.h>
#include <iris/core/vector3.h>
#include <iris/events/event.h>
#include <iris/events/keyboard_event.h>
#include <iris/graphics/mesh_manager.h>
#include <iris/graphics/render_entity.h>
#include <iris/graphics/render_graph/colour_node.h>
#include <iris/graphics/render_graph/render_graph.h>
#include <iris/graphics/render_graph/texture_node.h>
#include <iris/graphics/render_target.h>
#include <iris/graphics/renderer.h>
#include <iris/graphics/scene.h>
#include <iris/graphics/window.h>
#include <iris/graphics/window_manager.h>

namespace
{

/**
 * Helper function to update camera based on user input.
 *
 * @param camera
 *   Camera to update.
 *
 * @param key_map
 *   Map of user pressed keys.
 */
void update_camera(
    iris::Camera &camera,
    const std::map<iris::Key, iris::KeyState> &key_map)
{
    static auto speed = 2.0f;
    iris::Vector3 velocity{};

    if (key_map.at(iris::Key::W) == iris::KeyState::DOWN)
    {
        velocity += camera.direction() * speed;
    }

    if (key_map.at(iris::Key::S) == iris::KeyState::DOWN)
    {
        velocity -= camera.direction() * speed;
    }

    if (key_map.at(iris::Key::A) == iris::KeyState::DOWN)
    {
        velocity -= camera.right() * speed;
    }

    if (key_map.at(iris::Key::D) == iris::KeyState::DOWN)
    {
        velocity += camera.right() * speed;
    }

    if (key_map.at(iris::Key::Q) == iris::KeyState::DOWN)
    {
        velocity += camera.right().cross(camera.direction()) * speed;
    }

    if (key_map.at(iris::Key::E) == iris::KeyState::DOWN)
    {
        velocity -= camera.right().cross(camera.direction()) * speed;
    }

    camera.translate(velocity);
}

}

void go(int, char **)
{
    iris::ResourceLoader::instance().set_root_directory("assets");

    iris::Root::set_graphics_api("opengl");

    std::map<iris::Key, iris::KeyState> key_map = {
        {iris::Key::W, iris::KeyState::UP},
        {iris::Key::A, iris::KeyState::UP},
        {iris::Key::S, iris::KeyState::UP},
        {iris::Key::D, iris::KeyState::UP},
        {iris::Key::Q, iris::KeyState::UP},
        {iris::Key::E, iris::KeyState::UP},
    };

    auto window = iris::Root::window_manager().create_window(800u, 800u);
    iris::Camera camera{iris::CameraType::PERSPECTIVE, 800u, 800u};
    iris::Camera orth{iris::CameraType::ORTHOGRAPHIC, 800u, 800u};
    auto *rt = window->create_render_target(800u, 800u);
    camera.set_position({0.0f, 0.0f, 800.0f});

    auto &mesh_manager = iris::Root::mesh_manager();

    auto scene = std::make_unique<iris::Scene>();
    auto *light = scene->create_light<iris::DirectionalLight>(
        iris::Vector3{-1.0f, -1.0f, 0.0f});
    auto *e = scene->create_entity(
        nullptr,
        mesh_manager.cube(iris::Colour(1.0f, 0.0f, 1.0f)),
        iris::Transform{{0.5f, 0.0f, 0.0f}, {}, {100.0f}});

    auto *rg = scene->create_render_graph();
    rg->render_node()->set_colour_input(
        rg->create<iris::TextureNode>("crate.png"));

    scene->create_entity(
        rg,
        mesh_manager.cube(iris::Colour(1.0f, 0.0f, 0.0f)),
        iris::Transform{{100.0f, 100.0f, 0.0f}, {}, {100.0f}});
    scene->set_ambient_light({0.2f, 0.2f, 0.2f, 1.0f});

    iris::RenderPass pass1{scene.get(), &camera, nullptr};

    iris::Transform light_transform{light->direction(), {}, {1.0f}};

    window->set_render_passes({pass1});

    auto rot = 0.0f;
    auto running = true;

    do
    {
        auto event = window->pump_event();
        while (event)
        {
            std::cout << "handled" << std::endl;
            if (event->is_quit() || event->is_key(iris::Key::ESCAPE))
            {
                running = false;
                break;
            }
            else if (event->is_key())
            {
                const auto keyboard = event->key();
                key_map[keyboard.key] = keyboard.state;
            }
            else if (event->is_mouse())
            {
                static const auto sensitivity = 0.0025f;
                const auto mouse = event->mouse();

                camera.adjust_yaw(mouse.delta_x * sensitivity);
                camera.adjust_pitch(-mouse.delta_y * sensitivity);
            }

            event = window->pump_event();
        }

        update_camera(camera, key_map);

        rot += 0.01f;
        e->set_orientation({{0.0f, 1.0f, 0.0f}, rot});

        light_transform.set_matrix(
            iris::Matrix4(iris::Quaternion{{0.0f, 1.0f, 0.0f}, -0.01f}) *
            light_transform.matrix());
        light->set_direction(light_transform.translation());

        window->render();
        // window->render(pipeline2);
    } while (running);
}

int main(int argc, char **argv)
{
    iris::start_debug(argc, argv, go);

    return 0;
}
