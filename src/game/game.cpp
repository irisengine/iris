#include <tuple>
#include <cmath>
#include <chrono>
#include <cstdint>
#include <experimental/filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

#include "box.hpp"
#include "camera.hpp"
#include "entity.hpp"
#include "event_dispatcher.hpp"
#include "keyboard_event.hpp"
#include "mouse_event.hpp"
#include "physics_system.hpp"
#include "plane.hpp"
#include "quaternion.hpp"
#include "render_system.hpp"
#include "rigid_body.hpp"
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

void go([[maybe_unused]] int argc, char **argv)
{
    key_map[eng::key::W] = eng::key_state::UP;
    key_map[eng::key::A] = eng::key_state::UP;
    key_map[eng::key::S] = eng::key_state::UP;
    key_map[eng::key::D] = eng::key_state::UP;
    key_map[eng::key::Q] = eng::key_state::UP;
    key_map[eng::key::P] = eng::key_state::UP;
    key_map[eng::key::SPACE] = eng::key_state::UP;

    const auto width = 800.0f;
    const auto height = 800.0f;

    eng::event_dispatcher dispatcher{ keyevent_handler, mouseevent_handler };

    auto window = std::make_shared<eng::window>(dispatcher, width, height);

    camera = std::make_shared<eng::camera>();

    eng::render_system rs{ camera, window, width, height };
    eng::physics_system ps{ };

    rs.set_light_position({ 0.0f, 10.0f, 30.0f });

    std::vector<std::tuple<std::shared_ptr<eng::entity>, std::shared_ptr<eng::rigid_body>>> bodies{ };

    const auto add_body = [&bodies, &rs, &ps, &argv](
        const eng::vector3 &position,
        const bool is_static=false)
    {
        const auto mass = 1.0f;

        auto entity = std::make_shared<eng::entity>(
            std::experimental::filesystem::path{ argv[1] },
            eng::vector3{ },
            eng::vector3{ 1.0f, 1.0f, 1.0f });
        auto body = std::make_shared<eng::box>(
            position,
            mass,
            is_static);
        body->set_constant_acceleration({0.0f, -10.0f, 0.0f});

        bodies.emplace_back(std::make_tuple(entity, body));
        rs.add(entity);
        ps.add(body);
    };

    auto plane_body = std::make_shared<eng::plane>(
        eng::vector3{ 0.0f, 1.0f, 0.0f },
        0.0f);

    ps.add(plane_body);

    for(auto x = 0u; x < 5u; ++x)
    {
        for(auto y = 0u; y < 2u; ++y)
        {
            for(auto z = 0u; z < 5u; ++z)
            {
                add_body({x * 2.0f, 10.0f + (y * 2.0f), z * 2.0f });
            }
        }
    }

    camera->translate({0.0f, 10.0, 20.0f});

    auto wireframe = false;
    auto delta = 1.0f / 30.0f;

    while(key_map[eng::key::Q] == eng::key_state::UP)
    {
        const auto start = std::chrono::high_resolution_clock::now();

        static const float speed = 0.1f;

        auto direction = camera->direction();
        direction.y = 0.0f;

        auto &camera_body = std::get<1>(bodies.back());

        if(key_map[eng::key::SPACE] == eng::key_state::DOWN)
        {
            camera_body->add_impulse({ 0.0f, 2.0f, 0.0f });
        }

        if(key_map[eng::key::W] == eng::key_state::DOWN)
        {
            camera_body->add_impulse(direction * speed);
        }

        if(key_map[eng::key::S] == eng::key_state::DOWN)
        {
            camera_body->add_impulse(direction * -speed);
        }

        if(key_map[eng::key::A] == eng::key_state::DOWN)
        {
            camera_body->add_impulse(camera->right() * -speed);
        }

        if(key_map[eng::key::D] == eng::key_state::DOWN)
        {
            camera_body->add_impulse(camera->right() * speed);
        }

        if(key_map[eng::key::P] == eng::key_state::DOWN)
        {
            wireframe = !wireframe;
            rs.set_wireframe_mode(wireframe);
            key_map[eng::key::P] = eng::key_state::UP;
        }

        delta = 1.0f / 60.0f;

        ps.step(delta);

        for(auto i = 0u; i < bodies.size(); ++i)
        {
            const auto &[entity, body] = bodies[i];
            if(entity)
            {
                entity->set_model(body->transform());
            }
        }

        rs.render();

        const auto end = std::chrono::high_resolution_clock::now();

        const auto delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        delta = static_cast<float>(delta_ms.count() / 1000.0f);
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

