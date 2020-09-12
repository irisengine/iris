#include "core/looper.h"

#include <chrono>

namespace eng
{

Looper::Looper(
    std::chrono::microseconds clock,
    std::chrono::microseconds timestep,
    LoopFunction fixed_timestep,
    LoopFunction variable_timestep)
    : clock_(clock),
      timestep_(timestep),
      fixed_timestep_(fixed_timestep),
      variable_timestep_(variable_timestep)
{
}

void Looper::run()
{
    auto run = true;
    auto start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::duration accumulator(0);

    do
    {
        // calculate duration of last frame
        const auto end = std::chrono::steady_clock::now();
        const auto frame_time = end - start;
        start = end;

        // variable time step function produces time
        accumulator += frame_time;

        // fixed time step function consumed time
        while(run && (accumulator >= timestep_))
        {
            run &= fixed_timestep_(clock_, timestep_);

            accumulator -= timestep_;
            clock_ += timestep_;
        }

        run &= variable_timestep_(
            clock_,
            std::chrono::duration_cast<std::chrono::microseconds>(frame_time));
    } while(run);
}

}

