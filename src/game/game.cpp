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
#include "opengl_render_system.hpp"
#include "vector3.hpp"
#include "window.hpp"

std::shared_ptr<eng::camera> camera;
std::map<eng::key, eng::key_state> key_map;

void keyevent_handler(const eng::keyboard_event &event)
{
    key_map[event.key] = event.state;
}

void mouseevent_handler(const eng::mouse_event &event)
{
    static const auto sensitivity = 0.0025f;

    camera->adjust_yaw(-event.delta_x * sensitivity);
    camera->adjust_pitch(event.delta_y * sensitivity);
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

    eng::window w{ dispatcher, width, height };

    camera = std::make_shared<eng::camera>();
    eng::opengl_render_system rs{ camera, width, height };

    const std::vector<float> triangle_verts = {
        0.0f, 0.5f,
        0.5f, -0.5f,
       -0.5f, -0.5f
    };

    auto entity1 = std::make_shared<eng::entity>(
        std::experimental::filesystem::path{ argv[1] },
        0xFFFFFFFF,
        eng::vector3{ 0.0f, 0.0f, -100.0f },
        eng::vector3{ 10.0f, 10.0f, 10.0f });
    rs.add(entity1);

    camera->translate({ 0.0f, 120.0f, 0.0f });

    auto wireframe = false;

    while(key_map[eng::key::Q] == eng::key_state::UP)
    {
        static const float speed = 2.0f;

        auto direction = camera->direction();
        direction.y = 0.0f;


        if(key_map[eng::key::W] == eng::key_state::DOWN)
        {
            camera->translate(direction * -speed);
        }

        if(key_map[eng::key::S] == eng::key_state::DOWN)
        {
            camera->translate(direction * speed);
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

        w.pre_render();
        rs.render();
        w.post_render();
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

