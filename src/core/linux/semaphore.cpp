////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/semaphore.h"

#include <atomic>
#include <cstdint>
#include <memory>

#include <semaphore.h>

#include "core/auto_release.h"
#include "core/error_handling.h"

namespace iris
{

struct Semaphore::implementation
{
    sem_t semaphore;
    AutoRelease<sem_t *, nullptr> auto_semaphore;
    std::atomic<std::ptrdiff_t> count;
};

Semaphore::Semaphore(std::ptrdiff_t initial)
    : impl_(std::make_unique<implementation>())
{
    ensure(::sem_init(&impl_->semaphore, 0, initial) == 0, "could not create semaphore");

    impl_->auto_semaphore = {&impl_->semaphore, ::sem_destroy};
    impl_->count = initial;
}

Semaphore::~Semaphore() = default;
Semaphore::Semaphore(Semaphore &&) = default;
Semaphore &Semaphore::operator=(Semaphore &&) = default;

void Semaphore::release()
{
    ++impl_->count;
    expect(::sem_post(impl_->auto_semaphore) == 0, "coult not release semaphore");
}

void Semaphore::acquire()
{
    expect(::sem_wait(impl_->auto_semaphore) == 0, "could not acquire semaphore");
    --impl_->count;
}

}
