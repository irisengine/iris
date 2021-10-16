////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "jobs/fiber/counter.h"

#include <atomic>
#include <mutex>

namespace iris
{

Counter::Counter(int value)
    : value_(value)
    , mutex_()
{
}

Counter::operator int()
{
    std::unique_lock lock(mutex_);

    return value_;
}

void Counter::operator--()
{
    std::unique_lock lock(mutex_);

    --value_;
}

void Counter::operator--(int)
{
    std::unique_lock lock(mutex_);

    --value_;
}

}
