#include "core/semaphore.h"

#include <cstdint>
#include <memory>

#include <dispatch/dispatch.h>

#include "core/auto_release.h"

namespace iris
{

struct Semaphore::implementation
{
    AutoRelease<::dispatch_semaphore_t, nullptr> semaphore;
    std::atomic<std::ptrdiff_t> count;
};

Semaphore::Semaphore(std::ptrdiff_t initial)
    : impl_(std::make_unique<implementation>())
{
    impl_->semaphore = {
        ::dispatch_semaphore_create(initial), ::dispatch_release};
    impl_->count = initial;
}

Semaphore::~Semaphore() = default;
Semaphore::Semaphore(Semaphore &&) = default;
Semaphore &Semaphore::operator=(Semaphore &&) = default;

void Semaphore::release()
{
    ++impl_->count;
    ::dispatch_semaphore_signal(impl_->semaphore);
}

void Semaphore::acquire()
{
    ::dispatch_semaphore_wait(impl_->semaphore, DISPATCH_TIME_FOREVER);
    --impl_->count;
}

}
