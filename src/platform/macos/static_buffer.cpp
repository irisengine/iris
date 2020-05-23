#include "platform/static_buffer.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <set>

#include <sys/mman.h>
#include <unistd.h>

#include "core/exception.h"

namespace eng
{

struct StaticBuffer::implementation
{
    std::byte *allocated_region;
    std::size_t allocated_size;
    std::byte *usable_region;
    std::size_t usable_size;
};

StaticBuffer::StaticBuffer(std::size_t pages)
    : impl_(std::make_unique<implementation>())
{
    // calculate amount of bytes to allocate, including guard pages
    impl_->allocated_size = (pages + 2u) * page_size();

    impl_->allocated_region = static_cast<std::byte*>(::mmap(
        0,
        impl_->allocated_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANON,
        -1,
        0));

    if(impl_->allocated_region == MAP_FAILED)
    {
        throw Exception("failed to mmap memory");
    }

    // set head guard page
    if(::mprotect(
        impl_->allocated_region,
        page_size(),
        PROT_NONE) == -1)
    {
        throw Exception("failed to set head guard page");
    }

    // set tail guard page
    if(::mprotect(
        impl_->allocated_region + ((pages + 1u) * page_size()),
        page_size(),
        PROT_NONE) == -1)
    {
        throw Exception("failed to set tail guard page");
    }

    // calculate usable region pointer and size
    impl_->usable_region = impl_->allocated_region + page_size();
    impl_->usable_size = impl_->allocated_size - (2u * page_size());
}

StaticBuffer::~StaticBuffer()
{
    ::munmap(impl_->allocated_region, impl_->allocated_size);
}

std::size_t StaticBuffer::page_size()
{
    return static_cast<std::size_t>(::getpagesize());
}

StaticBuffer::operator std::byte*() const
{
    return impl_->usable_region;
}

std::size_t StaticBuffer::size() const
{
    return impl_->usable_size;
}

}

