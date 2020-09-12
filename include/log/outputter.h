#pragma once

#include <string>

namespace iris
{

/**
 * Interface for an outputter, a class which writes a log message to some
 * implementation defined medium.
 */
class Outputter
{
  public:
    /** Default */
    virtual ~Outputter() = default;

    /**
     * Output log.
     *
     * @param log
     *   Log message to output.
     */
    virtual void output(const std::string &log) = 0;
};

}
