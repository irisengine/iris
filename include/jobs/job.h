#pragma once

#include <any>
#include <functional>
#include <memory>
#include <iostream>

#include "log/log.h"

namespace eng
{

// convenient alias for a job
using Job = std::function<void()>;

}

