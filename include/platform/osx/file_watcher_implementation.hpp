#pragma once

#include <experimental/filesystem>
#include <vector>

#include <CoreServices/CoreServices.h>

#include "file_event_type.hpp"

namespace eng
{

/**
 * Implementation file for OSX file watcher.
 */
class file_watcher_implementation final
{
    public:

        /**
         * Construct a new file_watcher_implementation.
         *
         * @param path
         *   Path to watch.
         *
         * @param events
         *   Pointer to a collection where new events will be stored.
         */
        file_watcher_implementation(
            const std::experimental::filesystem::path &path,
            std::vector<file_event_type> * const events);

        /**
         * Destructor. Performs cleanup.
         */
        ~file_watcher_implementation();

        // Disabled
        file_watcher_implementation(const file_watcher_implementation&) = delete;
        file_watcher_implementation& operator=(const file_watcher_implementation&) = delete;

        /**
         * Move construct a new file_watcher_implementation.
         *
         * Other must not be used after this call.
         *
         * @param other
         *   The implementation to construct from.
         */
        file_watcher_implementation(file_watcher_implementation &&other);

        /**
         * Move assign a new file_watcher_implementation.
         *
         * Other must not be used after this call.
         *
         * @param other
         *   The implementation to assign from.
         *
         * @returns
         *   Reference to this implementation, after move assignment.
         */
        file_watcher_implementation& operator=(file_watcher_implementation &&other);

        /**
         * Swap this implementation with another.
         *
         * @param other
         *   Implementation to swap with.
         */
        void swap(file_watcher_implementation &other);

    private:

        /** Native file system event stream object. */
        FSEventStreamRef stream_;
};

}

