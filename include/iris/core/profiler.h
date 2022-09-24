////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>

namespace iris
{

/**
 * Sampling based profiler which periodically suspends and samples all running threads.
 * 
 * Currently prints the profile breakdown to stdout when program ends.
 */
class Profiler
{
  public:
    /**
     * Construct a new Profiler.
     */
    Profiler();
    
    /**
     * Signals profiling thread to stop
     */
    ~Profiler();

  private:
    /** Pointer to implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
