#pragma once

#include <cstdint>

#include "graphics/anti_aliasing_level.h"
#include "graphics/win32/win32_window.h"

namespace iris
{

/**
 * Implementation of Win32Window for OpenGL.
 */
class Win32OpenGLWindow : public Win32Window
{
  public:
    /**
     * Create a Win32OpenGLWindow.
     *
     * @param width
     *   Width of window.
     *
     * @param height
     *   Height of window.
     */
    Win32OpenGLWindow(
        std::uint32_t width,
        std::uint32_t height,
        AntiAliasingLevel anti_aliasing_level);

    ~Win32OpenGLWindow() override = default;
};

}
