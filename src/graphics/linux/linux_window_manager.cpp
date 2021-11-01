////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/linux/linux_window_manager.h"

#include <cstdint>
#include <memory>

#include "core/error_handling.h"
#include "core/root.h"
#include "graphics/linux/linux_window.h"
#include "graphics/linux/linux_window_manager.h"
#include "graphics/window.h"
#include "graphics/window_manager.h"

namespace iris
{

Window *LinuxWindowManager::create_window(std::uint32_t width, std::uint32_t height)
{
    // only support onw window at the moment
    ensure(!current_window_, "window already created");

    current_window_ = std::make_unique<LinuxWindow>(width, height);

    return current_window_.get();
}

Window *LinuxWindowManager::current_window() const
{
    return current_window_.get();
}
}
