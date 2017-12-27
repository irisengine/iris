#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "mesh.hpp"
#include "opengl_render_system.hpp"
#include "window.hpp"

void go()
{
    eng::window w(800.0f, 800.0f);

    eng::opengl_render_system rs{ };

    auto mesh1 = std::make_shared<eng::mesh>(std::vector<float>{
        0.0f,  0.75f,
        0.25f, 0.25f,
       -0.25f, 0.25f
    }, 0xFF000000);

    rs.add(mesh1);

    auto mesh2 = std::make_shared<eng::mesh>(std::vector<float>{
        0.25f,  0.25f,
        0.5f, -0.25f,
       0.0f, -0.25f
    }, 0x00FF0000);

    rs.add(mesh2);

    auto mesh3 = std::make_shared<eng::mesh>(std::vector<float>{
        -0.25f,  0.25f,
        -0.5f, -0.25f,
         0.0f, -0.25f
    }, 0x0000FF00);

    rs.add(mesh3);

    for(;;)
    {
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

