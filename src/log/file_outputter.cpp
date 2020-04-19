#include "log/file_outputter.h"

#include <string>

#include "core/exception.h"

namespace eng
{

FileOutputter::FileOutputter(const std::string &filename)
    : file_(filename, std::ios::out | std::ios::app)
{
    // better check all of these
    if(!file_.is_open() || file_.bad() || !file_.good() || file_.fail())
    {
        throw Exception("failed to open log file");
    }
}

void FileOutputter::output(const std::string &log)
{
    file_ << log << std::endl;
}

}

