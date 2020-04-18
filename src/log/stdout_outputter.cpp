#include "log/stdout_outputter.hpp"

#include <iostream>
#include <string>

namespace eng
{

void StdoutFormatter::output(const std::string &log)
{
    std::cout << log << std::endl;
}

}

