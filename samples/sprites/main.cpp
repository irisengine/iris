#include <iostream>

#include "core/quaternion.h"
#include "core/root.h"
#include "graphics/sprite.h"
#include "graphics/texture.h"
#include "graphics/texture_factory.h"
#include "log/log.h"
#include "platform/event.h"
#include "platform/start.h"

void go(int, char **)
{
    LOG_DEBUG("sprite_sample", "hello world");

    auto &rs = iris::Root::instance().render_system();

    auto *sprite1 = rs.create<iris::Sprite>(
        0, 0, 100, 100, iris::Vector3{0.39f, 0.58f, 0.92f});

    auto *sprite2 = rs.create<iris::Sprite>(
        0.0f, 150.0f, 100, 100, iris::Vector3{0.86f, 0.08f, 0.23f});

    auto *sprite3 = rs.create<iris::Sprite>(
        0.0f,
        -150.0f,
        100.0f,
        100.0f,
        iris::Vector3{1.0f, 1.0f, 1.0f},
        iris::texture_factory::load("circle.png"));

    iris::Quaternion rot{{0.0f, 0.0f, 1.0f}, 0.0f};
    iris::Quaternion delta{{0.0f, 0.0f, 1.0f}, 0.02f};

    for (;;)
    {
        if (auto evt = iris::Root::instance().window().pump_event(); evt)
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

        rs.render();
    }
    LOG_ERROR("sprite_sample", "goodbye!");
}

int main(int argc, char **argv)
{
    try
    {
        iris::start_debug(argc, argv, go);
    }
    catch (iris::Exception &e)
    {
        LOG_ERROR("sprite_sample", e.what());
        LOG_ERROR("sprite_sample", e.stack_trace());
    }
    catch (...)
    {
        LOG_ERROR("sprite_sample", "unknown exception");
    }
}
