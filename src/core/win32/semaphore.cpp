////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/semaphore.h"

#include <atomic>
#include <cstdint>
#include <memory>

#include <windows.h>

#include "core/auto_release.h"
#include "core/error_handling.h"

namespace iris
{

struct Semaphore::implementation
{
    AutoRelease<HANDLE, nullptr> semaphore;
};

Semaphore::Semaphore(std::ptrdiff_t initial)
    : impl_(std::make_unique<implementation>())
{
    impl_->semaphore = {::CreateSemaphoreA(NULL, static_cast<LONG>(initial), 10000u, NULL), ::CloseHandle};

    expect(impl_, "could not create semaphore");
}

Semaphore::~Semaphore() = default;
Semaphore::Semaphore(Semaphore &&) = default;
Semaphore &Semaphore::operator=(Semaphore &&) = default;

void Semaphore::release()
{
    const auto release = ::ReleaseSemaphore(impl_->semaphore, 1, NULL);
    expect(release != 0, "could not release semaphore");
}

void Semaphore::acquire()
{
    const auto wait = ::WaitForSingleObject(impl_->semaphore, INFINITE);
    expect(wait != WAIT_FAILED, "could not acquire semaphore");
}

}
