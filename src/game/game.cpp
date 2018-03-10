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

    rs.set_light_position({ 0.0f, 50.0f, 30.0f });

    std::vector<std::tuple<std::shared_ptr<eng::entity>, std::shared_ptr<eng::rigid_body>>> bodies{ };

    [[maybe_unused]] const auto add_body = [&bodies, &rs, &ps, &argv](
        const eng::vector3 &position,
        const bool is_static=false,
        const eng::vector3 &half_size=eng::vector3(1.0f, 1.0f, 1.0f),
        const eng::quaternion &orientation=eng::quaternion())
    {
        const auto mass = 1.0f;

        auto entity = std::make_shared<eng::entity>(
            std::experimental::filesystem::path{ argv[4] },
            eng::vector3{ },
            eng::quaternion{ },
            eng::vector3{ 1.0f, 1.0f, 1.0f });
        auto body = std::make_shared<eng::box>(
            position,
            mass,
            half_size,
            is_static);
        body->set_constant_acceleration({0.0f, -10.0f, 0.0f});
        body->warp_orientation(orientation);

        bodies.emplace_back(std::make_tuple(entity, body));
        rs.add(entity);
        ps.add(body);
    };

    auto plane_body = std::make_shared<eng::plane>(
        eng::vector3{ 0.0f, 1.0f, 0.0f },
        0.0f);

    ps.add(plane_body);

    for(auto i = 0u; i < 5u; ++i)
    {
        for(auto j = 0u; j < 2u; ++j)
        {
            for(auto k = 0u; k < 5u; ++k)
            {
                //add_body({i * 2.0f, 10.0f + (j * 2.0f), (k * 2.0f)});
            }
        }
    }

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

    camera->translate({ 0.0f, 5.0f, 20.0f });
    for(const auto &[pos, angle, index] : corridors)
    {
        const auto scale = 10.0f;
        auto entity = std::make_shared<eng::entity>(
            std::experimental::filesystem::path{ argv[index] },
            pos * scale,
            eng::quaternion{{ 0.0f, 1.0f, 0.0f}, angle },
            eng::vector3{ scale, scale, scale });
        rs.add(entity);
        entity->set_wireframe(false);
    }

    auto camera_body = std::make_shared<eng::box>(
        eng::vector3{ 0.0f, 3.0f, 20.0f },
        1.0f,
        eng::vector3{ 1.0f, 3.0f, 1.0f },
        false);
    camera_body->set_constant_acceleration({0.0f, -10.0f, 0.0f});
    camera_body->set_angular_factor({0.0f, 0.0f, 0.0f});
    camera_body->set_linear_damping(0.1f);
    ps.add(camera_body);

    auto player = std::make_shared<eng::entity>(
        std::experimental::filesystem::path{ argv[4] },
        eng::vector3{ },
        eng::quaternion{ },
        eng::vector3{ 1.0f, 3.0f, 1.0f });
    player->set_wireframe(true);
    rs.add(player);

    std::vector<std::tuple<eng::vector3, eng::vector3>> walls {
        { { -70.0f, 0.0f, -40.0f }, { 62.0f, 10.0f, 62.0f } },
        { { 18.0f, 0.0f, -40.0f }, { 10.0f, 10.0f, 80.0f } },
        { { -158.0f, 0.0f, -40.0f }, { 10.0f, 10.0f, 80.0f } },
        { { -70.0f, 0.0f, -180.0f }, { 80.0f, 10.0f, 62.0f } },
        { { -70.0f, 0.0f, 100.0f }, { 80.0f, 10.0f, 62.0f } },
    };

    for(const auto &[pos, half_size] : walls)
    {
        auto wall_body = std::make_shared<eng::box>(
            pos,
            1.0f,
            half_size,
            true);
        ps.add(wall_body);

        auto wall_entity = std::make_shared<eng::entity>(
            std::experimental::filesystem::path{ argv[4] },
            eng::vector3{ },
            eng::quaternion{ },
            half_size);
        wall_entity->set_wireframe(true);
        wall_entity->set_position(wall_body->position());
        wall_entity->set_orientation(wall_body->orientation());
        rs.add(wall_entity);
    }

    auto delta = 1.0f / 30.0f;

    while(key_map[eng::key::Q] == eng::key_state::UP)
    {
        const auto start = std::chrono::high_resolution_clock::now();

        static float speed = 1.0;

        auto direction = camera->direction();
        direction.y = 0.0f;

        if(key_map[eng::key::SPACE] == eng::key_state::DOWN)
        {
            speed = 4.0f;
        }
        else
        {
            speed = 1.0f;
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

        delta = 1.0f / 60.0f;

        ps.step(delta);

        for(auto i = 0u; i < bodies.size(); ++i)
        {
            const auto &[entity, body] = bodies[i];
            if(entity)
            {
                entity->set_position(body->position());
                entity->set_orientation(body->orientation());
            }
        }

        rs.render();

        const auto end = std::chrono::high_resolution_clock::now();

        const auto delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        delta = static_cast<float>(delta_ms.count() / 1000.0f);

        player->set_position(camera_body->position());
        player->set_orientation(camera_body->orientation());
        camera->translate(camera_body->position() - camera->position());
        camera->translate({0.0f, 1.0f, 0.0f});
        rs.set_light_position(camera_body->position());
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

