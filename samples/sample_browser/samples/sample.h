#pragma once

#include <string>

#include <events/event.h>

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
};
