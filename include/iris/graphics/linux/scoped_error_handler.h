#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace iris
{

/**
 * This struct sets a new X11 error handler to record errors. Restores original handler at end of scope.
 */
class ScopedErrorHandler
{
  public:
    /**
     * Construct a new ScopedErrorHandler, sets a new handler.
     */
    ScopedErrorHandler()
        : old_handler_(nullptr)
    {
        // set handler and store old one.
        old_handler_ = ::XSetErrorHandler(error_handler);
    }

    /**
     * Restores old handler.
     */
    ~ScopedErrorHandler()
    {
        ::XSetErrorHandler(old_handler_);
    }

    /**
     * Checks if an error has occurred.
     *
     * @returns
     *   True if an error has occurred, false otherwise.
     */
    operator bool() const
    {
        return error;
    }

    /**
     * Reset to a "no error" state.
     */
    void reset()
    {
        error = false;
    }

  private:
    /** Flag indicating if an error has occurred */
    static bool error;

    /**
     * Error handler function.
     *
     * @returns
     *   Always returns 0.
     */
    static int error_handler(Display *, XErrorEvent *)
    {
        error = true;
        return 0;
    }

    /** Original error handler function. */
    int (*old_handler_)(Display *, XErrorEvent *);
};

}
