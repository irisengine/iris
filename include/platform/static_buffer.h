#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <set>

namespace eng
{

/**
 * Fixed size managed buffer. Memory is allocated in pages and is read-writable.
 * Guard pages are also allocated before and after the memory region, so
 * out-of-bounds access fails fast and loud.
 */
class StaticBuffer
{
    public:

        /**
         * Create buffer with requested number of pages.
         *
         * @param pages
         *   Number of pages to allocate.
         */
        explicit StaticBuffer(std::size_t pages);

        /**
         * Release all allocated memory.
         */
        ~StaticBuffer();

        /**
         * Get page size in bytes.
         *
         * @returns
         *   Number of bytes in a page.
         */
        static std::size_t page_size();

        /**
         * Get start of allocated buffer.
         *
         * @returns
         *   Pointer to start of buffer.
         */
        operator std::byte*() const;

        /**
         * Number of allocated bytes. This does not include any guard pages.
         *
         * @returns
         *   Number of allocated bytes.
         */
        std::size_t size() const;

    private:

        /** Pointer to implementation. */
        struct implementation;
        std::unique_ptr<implementation> impl_;

};

}

