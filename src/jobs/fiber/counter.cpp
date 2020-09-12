#include "jobs/fiber/counter.h"

#include <atomic>
#include <mutex>

#include "core/exception.h"

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
