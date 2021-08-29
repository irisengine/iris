#include "core/semaphore.h"

#include <atomic>
#include <cstdint>
#include <memory>

#include <windows.h>

#include "core/auto_release.h"
#include "core/exception.h"

namespace iris
{

struct Semaphore::implementation
{
    AutoRelease<HANDLE, nullptr> semaphore;
};

Semaphore::Semaphore(std::ptrdiff_t initial)
    : impl_(std::make_unique<implementation>())
{
    impl_->semaphore = {
        ::CreateSemaphoreA(NULL, static_cast<LONG>(initial), 10000u, NULL),
        ::CloseHandle};

    if (!impl_)
    {
        throw Exception("could not create semaphore");
    }
}

Semaphore::~Semaphore() = default;
Semaphore::Semaphore(Semaphore &&) = default;
Semaphore &Semaphore::operator=(Semaphore &&) = default;

void Semaphore::release()
{
    if (::ReleaseSemaphore(impl_->semaphore, 1, NULL) == FALSE)
    {
        throw Exception("could not release semaphore");
    }
}

void Semaphore::acquire()
{
    if (::WaitForSingleObject(impl_->semaphore, INFINITE) == WAIT_FAILED)
    {
        throw Exception("could not acquire semaphore");
    }
}

}
