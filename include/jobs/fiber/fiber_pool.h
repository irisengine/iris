#include <cstddef>
#include <iterator>
#include <list>

#include "core/exception.h"
#include "jobs/fiber/fiber.h"
#include "log/log.h"

namespace eng
{

/**
 * Pool of allocated fibers. Designed to minimise constant allocation and
 * deallocation of fiber objects.
 *
 * Internally fibers are stored in a doubly-linked list. When a new fiber is
 * requested the head of the list is returned and the head is advanced. When
 * a fiber is released the head is rewound one and the fiber is put into head.
 *
 *
 * +-------+      +-------+      +-------+
 * |#######|----->| free  |----->| free  |
 * |#######|<-----| fiber |<-----| fiber |
 * +-------+      +-------+      +-------+
 *                    .
 *                   / \
 *                    |
 *                    '------ head
 *
 * If the pool is empty then it is expanded by PoolBlockSize amount.
 */
template<std::size_t PoolBlockSize>
class FiberPool
{
    public:

        /**
         * Create a new pool with an initial amount of fibers.
         */
        FiberPool()
        {
            for(int i = 0; i < PoolBlockSize; ++i)
            {
                fibers.emplace_back(new eng::Fiber{ });
            }

            count = PoolBlockSize;
            
            head = std::begin(fibers);
        }

        /**
         * Delete all fibers.
         */
        ~FiberPool()
        {
            for(const auto &fiber: fibers)
            {
                if(fiber != nullptr)
                {
                    delete fiber;
                }
                else
                {
                    LOG_ENGINE_WARN("fiber_pool", "fiber not released");
                }
            }
        }

        // disable copying/moving
        FiberPool(const FiberPool&) = delete;
        FiberPool& operator=(const FiberPool&) = delete;
        FiberPool(FiberPool&&) = delete;
        FiberPool& operator=(FiberPool&&) = delete;

        /**
         * Get the next free fiber.
         *
         * @returns
         *   Pointer to a fiber.
         */
        eng::Fiber* next()
        {
            std::unique_lock lock(m);

            // get fiber from head
            auto f = *head;
            *head = nullptr;

            // if this is the last fiber then grow the pool
            if(head == std::prev(std::cend(fibers)))
            {
                for(int i = 0; i < PoolBlockSize; ++i)
                {
                    fibers.emplace_back(new eng::Fiber{ });
                }

                count += PoolBlockSize;
            }

            ++head;

            return f;
        }

        /**
         * Return a fiber to the pool. For efficiency the fiber is left
         * untouched e.g. its stack is not zeroed.
         *
         * @param fiber
         *   Fiber to release.
         */
        void release(eng::Fiber *fiber)
        {
            fiber->finish();

            std::unique_lock lock(m);

            if(head == std::cbegin(fibers))
            {
                throw Exception("a fiber has been released that was not allocated");
            }

            // rewind head and put fiber back
            --head;
            *head = fiber;
        }

        /**
         * Get the total number of fibers allocated.
         *
         * Note that this is *not* thread-safe as is for diagnostics only.
         *
         * @returns
         *   Number of fibers allocated.
         */
        std::size_t capacity() const
        {
            return count;
        }

    private:

        /** Mutex for control access. */
        std::mutex m;
        
        /** List of fibers in pool. */
        std::list<eng::Fiber*> fibers;

        /** Head of free fibers. */
        std::list<eng::Fiber*>::iterator head;
        
        /* Number of fibers allocated. */
        std::size_t count;
};

// alias for a pool with a default size
using DefaultFiberPool = FiberPool<1000u>;

}

