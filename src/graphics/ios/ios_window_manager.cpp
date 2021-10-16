////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/ios/ios_window_manager.h"

#include <cstdint>
#include <memory>

#include "core/error_handling.h"
#include "graphics/ios/ios_window.h"

namespace iris
{

Window *IOSWindowManager::create_window(std::uint32_t width, std::uint32_t height)
{
    ensure(!current_window_, "window already created");

    current_window_ = std::make_unique<IOSWindow>(width, height);
    return current_window_.get();
}

Window *IOSWindowManager::current_window() const
{
    return current_window_.get();
}

}
