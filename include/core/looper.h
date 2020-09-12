#pragma once

#include <chrono>
#include <functional>

namespace eng
{

/**
 * This class provides a game looper. It takes two functions, one that is called
 * at a fixed time step and another that is run as frequently as possible. This
 * is based on the https://gafferongames.com/post/fix_your_timestep/ article.
 */
class Looper
{
    public:

        /**
         * Definition of a function to run in loop.
         * 
         * @param clock
         *   Total elapsed time since loop started.
         * 
         * @param delta
         *   Duration of frame.
         * 
         * @returns
         *   True if loop should continue, false if it should exit.
         */
        using LoopFunction = std::function<bool(std::chrono::microseconds, std::chrono::microseconds)>;

        /**
         * Construct a new looper.
         * 
         * @param clock
         *   Start time of looping.
         * 
         * @param timestep
         *   How frequently to call the fixed time step function.
         * 
         * @param fixed_timestep
         *   Function to call at the supplied fixed timestep.
         * 
         * @param variable_timestep
         *   Function to call as frequently as possible.
         */
        Looper(
            std::chrono::microseconds clock,
            std::chrono::microseconds timestep,
            LoopFunction fixed_timestep,
            LoopFunction variable_timestep);

        /**
         * Run the loop. Will continue until one of the supplied functions
         * returns false. Clock time will start incrementing from this call.
         */
        void run();

    private:

        /** Elapsed time of loop. */
        std::chrono::microseconds clock_;

        /** Fixed time step. */
        std::chrono::microseconds timestep_;

        /** Function to run at foxed time step. */
        LoopFunction fixed_timestep_;

        /** Function to run at variable time step. */
        LoopFunction variable_timestep_;
};

}
