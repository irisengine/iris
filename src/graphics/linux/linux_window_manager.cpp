////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/linux/linux_window_manager.h"

#include <cstdint>
#include <memory>

#include "core/error_handling.h"
#include "graphics/linux/linux_window.h"
#include "graphics/linux/linux_window_manager.h"
#include "graphics/window.h"
#include "graphics/window_manager.h"

namespace iris
{

LinuxWindowManager::LinuxWindowManager(TextureManager &texture_manager, MaterialManager &material_manager)
    : texture_manager_(texture_manager)
    , material_manager_(material_manager)
    , current_window_()
{
}

Window *LinuxWindowManager::create_window(std::uint32_t width, std::uint32_t height)
{
    // only support one window at the moment
    ensure(!current_window_, "window already created");

    current_window_ = std::make_unique<LinuxWindow>(width, height, *this, texture_manager_, material_manager_);

    return current_window_.get();
}

Window *LinuxWindowManager::current_window() const
{
    return current_window_.get();
}
}
