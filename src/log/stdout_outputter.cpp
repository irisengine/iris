#include "log/stdout_outputter.h"

#include <iostream>
#include <string>

namespace eng
{

void StdoutFormatter::output(const std::string &log)
{
    std::cout << log << std::endl;
}

}

