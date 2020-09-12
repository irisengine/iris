#include "log/stdout_outputter.h"

#include <iostream>
#include <string>

namespace iris
{

void StdoutFormatter::output(const std::string &log)
{
    std::cout << log << std::endl;
}

}
