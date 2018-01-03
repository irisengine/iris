#include <cstdint>
#include <experimental/filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

#include "camera.hpp"
#include "entity.hpp"
#include "event_dispatcher.hpp"
#include "keyboard_event.hpp"
#include "mouse_event.hpp"
#include "render_system.hpp"
#include "vector3.hpp"
#include "window.hpp"

std::shared_ptr<eng::camera> camera;
std::map<eng::key, eng::key_state> key_map;

void keyevent_handler(const eng::keyboard_event &event)
{
    key_map[event.key] = event.state;

    if((event.key == eng::key::SPACE) && (event.state == eng::key_state::DOWN))
    {
        std::cout << camera->position() << std::endl;
    }
}

void mouseevent_handler(const eng::mouse_event &event)
{
    static const auto sensitivity = 0.0025f;

    camera->adjust_yaw(event.delta_x * sensitivity);
    camera->adjust_pitch(-event.delta_y * sensitivity);
}

void go(int argc, char **argv)
{
    key_map[eng::key::W] = eng::key_state::UP;
    key_map[eng::key::A] = eng::key_state::UP;
    key_map[eng::key::S] = eng::key_state::UP;
    key_map[eng::key::D] = eng::key_state::UP;
    key_map[eng::key::Q] = eng::key_state::UP;
    key_map[eng::key::P] = eng::key_state::UP;

    const auto width = 800.0f;
    const auto height = 800.0f;

    eng::event_dispatcher dispatcher{ keyevent_handler, mouseevent_handler };

    auto window = std::make_shared<eng::window>(dispatcher, width, height);

    camera = std::make_shared<eng::camera>();
    eng::render_system rs{ camera, window, width, height };

    rs.set_light_position({ 5.0f, 5.0f, 0.0f });

    const std::vector<float> triangle_verts = {
        0.0f, 0.5f,
        0.5f, -0.5f,
       -0.5f, -0.5f
    };

    static const auto pi = 3.141592654f;

    std::vector<std::tuple<eng::vector3, float, int>> corridors{
        { { 0.0f, 0.0f, 0.0f }, pi / 2.0f, 1 },
        { { 0.0f, 0.0f, -4.0f }, pi / 2.0f, 2 },
        { { 0.0f, 0.0f, -8.0f }, pi / 2.0f, 1 },
        { { -3.0f, 0.0f, -11.0f }, 0.0f, 1 },
        { { -7.0f, 0.0f, -11.0f }, pi, 2 },
        { { -11.0f, 0.0f, -11.0f }, 0.0f, 1 },
        { { -14.0f, 0.0f, 0.0f }, pi / 2.0f, 1 },
        { { -14.0f, 0.0f, -4.0f }, -pi / 2.0f, 2 },
        { { -14.0f, 0.0f, -8.0f }, pi / 2.0f, 1 },
        { { -3.0f, 0.0f, 3.0f }, 0.0f, 1 },
        { { -7.0f, 0.0f, 3.0f }, 0.0f, 2 },
        { { -11.0f, 0.0f, 3.0f }, 0.0f, 1 },
        { { 0.0f, 0.0f, -11.0f }, pi, 3 },
        { { -14.0f, 0.0f, -11.0f }, -pi / 2.0f, 3 },
        { { 0.0f, 0.0f, 3.0f }, pi / 2.0f, 3 },
        { { -14.0f, 0.0f, 3.0f }, 0.0f, 3 },
    };

    for(const auto &[pos, angle, index] : corridors)
    {
        auto entity = std::make_shared<eng::entity>(
            std::experimental::filesystem::path{ argv[index] },
            eng::vector3{ 0.0f, 0.0f, 0.0f },
            eng::vector3{ 1.0f, 1.0f, 1.0f });
        rs.add(entity);

        entity->rotate_y(angle);
        entity->translate(pos);
    }

    auto planet = std::make_shared<eng::entity>(
        std::experimental::filesystem::path{ argv[4] },
        eng::vector3{ 60.0f, -10.0f, -4.0f },
        eng::vector3{ 20.5f, 20.5f, 20.5f });
    rs.add(planet);

    auto wireframe = false;

    while(key_map[eng::key::Q] == eng::key_state::UP)
    {
        static const float speed = 0.1f;

        auto direction = camera->direction();
        direction.y = 0.0f;

        if(key_map[eng::key::W] == eng::key_state::DOWN)
        {
            camera->translate(direction * speed);
        }

        if(key_map[eng::key::S] == eng::key_state::DOWN)
        {
            camera->translate(direction * -speed);
        }

        if(key_map[eng::key::A] == eng::key_state::DOWN)
        {
            camera->translate(camera->right() * -speed);
        }

        if(key_map[eng::key::D] == eng::key_state::DOWN)
        {
            camera->translate(camera->right() * speed);
        }

        if(key_map[eng::key::P] == eng::key_state::DOWN)
        {
            wireframe = !wireframe;
            rs.set_wireframe_mode(wireframe);
            key_map[eng::key::P] = eng::key_state::UP;
        }

        rs.render();
    }
}

int main(int argc, char **argv)
{
    try
    {
        go(argc, argv);
    }
    catch(std::runtime_error &err)
    {
        std::cerr << err.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "unknown exception thrown" << std::endl;
    }
}

