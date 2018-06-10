#pragma once

#include <cstdint>
#include <experimental/filesystem>
#include <memory>
#include <vector>

#include "file_event_type.hpp"
#include "osx/file_watcher_implementation.hpp"

namespace eng
{

/**
 * Class used to monitor and report changes to a file.
 */
class file_watcher final
{
    public:

        /**
         * Construct a new file_watcher. Throws an expception of path does not
         * exist.
         *
         * @param path
         *   Path to watch.
         */
        file_watcher(const std::experimental::filesystem::path &path);

        /**
         * Check if events are available.
         *
         * @returns
         *   True if events are available, false otherwise.
         */
        bool has_events() const;

        /**
         * Yield all events, after this has_events() will return false
         *
         * @returns
         *   All events since last yield.
         */
        std::vector<file_event_type> yield();

    private:

        /** Collection of events. */
        std::vector<file_event_type> events_;

        /** Pointer to platform specific implementation. */
        std::unique_ptr<file_watcher_implementation> impl_;

};

}

