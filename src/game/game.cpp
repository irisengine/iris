#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "camera.hpp"
#include "event_dispatcher.hpp"
#include "keyboard_event.hpp"
#include "mesh.hpp"
#include "opengl_render_system.hpp"
#include "vector3.hpp"
#include "window.hpp"

void keyevent_handler(const eng::keyboard_event &event)
{
    std::cout << std::hex << static_cast<std::uint32_t>(event.key) << " " << static_cast<std::uint32_t>(event.state) << std::endl;
    std::cout << std::dec;
}

void go()
{
    const auto width = 800.0f;
    const auto height = 800.0f;

    eng::event_dispatcher dispatcher{ keyevent_handler };

    eng::window w{ dispatcher, width, height };

    auto camera = std::make_shared<eng::camera>();
    eng::opengl_render_system rs{ camera, width, height };

    const std::vector<float> triangle_verts = {
        0.0f, 0.5f,
        0.5f, -0.5f,
       -0.5f, -0.5f
    };

    auto mesh1 = std::make_shared<eng::mesh>(
        triangle_verts,
        0xFF000000,
        eng::vector3{ 0.0f, 0.0f, -100.0f },
        10.0f);
    rs.add(mesh1);

    auto mesh2  = std::make_shared<eng::mesh>(
        triangle_verts,
        0x00FF0000,
       eng::vector3{  0.0f, 0.0f, -100.0f },
       10.0f);
    rs.add(mesh2);

    auto mesh3 = std::make_shared<eng::mesh>(
        triangle_verts,
        0x0000FF00,
       eng::vector3{ 0.0f, 0.0f, -100.0f },
       10.0f);
    rs.add(mesh3);

    for(;;)
    {
        mesh1->translate(eng::vector3{ 1.1f, 0.0f, 0.0f });
        mesh2->translate(eng::vector3{ 0.0f, 1.1f, 0.0f });
        mesh3->translate(eng::vector3{ 0.0f, 0.0f, -1.0f });

        w.pre_render();
        rs.render();
        w.post_render();
    }
}

int main()
{
    try
    {
        go();
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

