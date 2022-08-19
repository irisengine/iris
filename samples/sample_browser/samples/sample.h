////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#include <events/event.h>
#include <graphics/render_target.h>
#include <graphics/renderer.h>

/**
 * Interface for a sample in the sample browser.
 */
class Sample
{
  public:
    virtual ~Sample() = default;

    /**
     * Fixed rate update function e.g. physics.
     */
    virtual void fixed_update() = 0;

    /**
     * Variable rate update function e.g. rendering.
     */
    virtual void variable_update() = 0;

    /**
     * Handle a user input.
     *
     * @param event
     *   User input event.
     */
    virtual void handle_input(const iris::Event &event) = 0;

    /**
     * Title of sample.
     *
     * @returns
     *   Sample title.
     */
    virtual std::string title() const = 0;

    /**
     * Get the target the sample will render to.
     *
     * @returns
     *   Sample render target.
     */
    virtual const iris::RenderTarget *target() const = 0;
};
