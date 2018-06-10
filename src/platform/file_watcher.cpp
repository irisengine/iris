#include "file_watcher.hpp"

#include <thread>
#include <experimental/filesystem>
#include <vector>

#include "exception.hpp"
#include "file_event_type.hpp"
#include "osx/file_watcher_implementation.hpp"

namespace eng
{

file_watcher::file_watcher(const std::experimental::filesystem::path &path)
    : events_(),
      impl_()
{
    if(!std::experimental::filesystem::exists(path))
    {
        throw exception("file does not exist");
    }

    impl_ = std::make_unique<file_watcher_implementation>(path, &events_);
}

bool file_watcher::has_events() const
{
    return !events_.empty();
}

std::vector<file_event_type> file_watcher::yield()
{
    const auto old = events_;

    events_.clear();

    return old;
}

}

