#include <map>

#include "core/camera.h"
#include "core/root.h"
#include "graphics/model.h"
#include "graphics/shape_factory.h"
#include "graphics/sprite.h"
#include "log/log.h"
#include "platform/keyboard_event.h"
#include "platform/start.h"
#include "platform/window.h"

void go(int, char **)
{
    LOG_DEBUG("cube_sample", "hello world");

    std::map<eng::Key, eng::KeyState> key_map {
        { eng::Key::W, eng::KeyState::UP },
        { eng::Key::A, eng::KeyState::UP },
        { eng::Key::S, eng::KeyState::UP },
        { eng::Key::D, eng::KeyState::UP },
        { eng::Key::Q, eng::KeyState::UP },
        { eng::Key::E, eng::KeyState::UP },
    };

    auto &rs = eng::Root::instance().render_system();
    auto &camera = rs.persective_camera();

    rs.create<eng::Model>(
        eng::Vector3{ -20.0f, 0.0f, 0.0f },
        eng::Vector3{ 10.0f, 10.0f, 10.0f },
        eng::shape_factory::cube({ 1.0f, 0.0f, 0.0f }));

    rs.create<eng::Model>(
        eng::Vector3{ 20.0f, 0.0f, 0.0f },
        eng::Vector3{ 10.0f, 10.0f, 10.0f },
        eng::shape_factory::cube({ 1.0f, 0.0f, 0.0f }));

    for(;;)
    {
        if(auto evt = eng::Root::instance().window().pump_event() ; evt)
        {
            if(evt->is_key(eng::Key::ESCAPE))
            {
                break;
            }
            else if(evt->is_key())
            {
                const auto keyboard = evt->key();
                key_map[keyboard.key] = keyboard.state;
            }
            else if(evt->is_mouse())
            {
                static const auto sensitivity = 0.0025f;
                const auto mouse = evt->mouse();

                camera.adjust_yaw(mouse.delta_x * sensitivity);
                camera.adjust_pitch(-mouse.delta_y * sensitivity);
            }
        }

        static auto speed = 2.0f;
        eng::Vector3 velocity;

        if(key_map[eng::Key::W] == eng::KeyState::DOWN)
        {
            velocity += camera.direction() * speed;
        }

        if(key_map[eng::Key::S] == eng::KeyState::DOWN)
        {
            velocity -= camera.direction() * speed;
        }

        if(key_map[eng::Key::A] == eng::KeyState::DOWN)
        {
            velocity -= camera.right() * speed;
        }

        if(key_map[eng::Key::D] == eng::KeyState::DOWN)
        {
            velocity += camera.right() * speed;
        }

        if(key_map[eng::Key::Q] == eng::KeyState::DOWN)
        {
            velocity += camera.right().cross(camera.direction()) * speed;
        }

        if(key_map[eng::Key::E] == eng::KeyState::DOWN)
        {
            velocity -= camera.right().cross(camera.direction()) * speed;
        }

        camera.translate(velocity);

        rs.render();
    }
    LOG_ERROR("cube_sample", "goodbye!");
}

int main(int argc, char **argv)
{
    try
    {
        eng::start_debug(argc, argv, go);
    }
    catch(eng::Exception &e)
    {
        LOG_ERROR("cube_sample", e.what());
        LOG_ERROR("cube_sample", e.stack_trace());
    }
    catch(...)
    {
        LOG_ERROR("cube_sample", "unknown exception");
    }

    return 0;
}

