#include <iostream>

#include "window.hpp"

int main()
{
    eng::window w{ 800.0f, 800.0f };

    for(;;)
    {
        w.pre_render();
        w.post_render();
    }

    return 0;
}

