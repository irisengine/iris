////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/macos/macos_window_manager.h"

#include <cstdint>
#include <memory>

#include "core/error_handling.h"
#include "graphics/macos/macos_window.h"

namespace iris
{

MacosWindowManager::MacosWindowManager(TextureManager &texture_manager, MaterialManager &material_manager)
    : texture_manager_(texture_manager)
    , material_manager_(material_manager)
    , current_window_()
{
}

Window *MacosWindowManager::create_window(std::uint32_t width, std::uint32_t height)
{
    ensure(!current_window_, "window already created");

    current_window_ = std::make_unique<MacosWindow>(width, height, texture_manager_, material_manager_);
    return current_window_.get();
}

Window *MacosWindowManager::current_window() const
{
    return current_window_.get();
}

}
