#pragma once

#include <cstdint>
#include <optional>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "core/auto_release.h"
#include "events/event.h"
#include "graphics/window.h"

namespace iris
{

/**
 * Abstract implementation of Window for win32. This class has concrete
 * implementations for each supported graphics api.
 */
class Win32Window : public Window
{
  public:
    // helper aliases
    using AutoWindow = iris::AutoRelease<HWND, NULL>;
    using AutoDC = iris::AutoRelease<HDC, NULL>;

    /**
     * Construct a new Win32Window.
     *
     * @param width
     *   Width of window.
     *
     * @param height
     *   Height of window.
     */
    Win32Window(std::uint32_t width, std::uint32_t height);
    ~Win32Window() override = default;

    /**
     * Get the natural scale for the screen. This value reflects the scale
     * factor needed to convert from the default logical coordinate space into
     * the device coordinate space of this screen.
     *
     * @returns
     *   Screen scale factor.
     */
    std::uint32_t screen_scale() const override;

    /**
     * Pump the next user input event. Result will be empty if there are no
     * new events.
     *
     * @returns
     *   Optional event.
     */
    std::optional<Event> pump_event() override;

    /**
     * Get device context handle.
     *
     * @returns
     *   Device context handle.
     */
    HDC device_context() const;

  protected:
    /** Win32 window handle. */
    AutoWindow window_;

    /** Win32 device context handle. */
    AutoDC dc_;

    /** Win32 window class object. */
    WNDCLASSA wc_;
};

}
