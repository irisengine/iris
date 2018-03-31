#include "file_outputter.hpp"

#include <string>

#include "exception.hpp"

namespace eng
{

file_outputter::file_outputter(const std::string &filename)
    : file_(filename, std::ios::out | std::ios::app)
{
    // better check all of these
    if(!file_.is_open() || file_.bad() || !file_.good() || file_.fail())
    {
        throw exception("failed to open log file");
    }
}

void file_outputter::output(const std::string &log)
{
    file_ << log << std::endl;
}

}

