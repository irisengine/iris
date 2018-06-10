#include "osx/file_watcher_implementation.hpp"

#import <CoreFoundation/CoreFoundation.h>
#import <CoreServices/CoreServices.h>

#include <experimental/filesystem>
#include <vector>

#include "file_event_type.hpp"
#include "osx/cf_ptr.hpp"

namespace
{

/**
 * Callback function which gets fired when a file event happens.
 *
 * @param client_callback_info
 *   Pointer to the callback data that was specified when the event stream was
 *   created.
 *
 * @param num_evemts
 *   The number of events since last callback.
 *
 * @param event_call_paths
 *   Array of c-string paths of files that have had an event.
 */
void callback(
    ConstFSEventStreamRef,
    void *client_callback_info,
    size_t num_events,
    void *event_call_paths,
    const FSEventStreamEventFlags*,
    const FSEventStreamEventId*)
{
    // cast callback data to collection pointer
    auto *events =
        static_cast<std::vector<eng::file_event_type> * const>(client_callback_info);

    // cast paths to c-strings
    const auto *paths = static_cast<char const * const * const>(event_call_paths);

    for (auto i = 0u; i < num_events; ++i)
    {
        const std::experimental::filesystem::path path{ paths[i] };

        // if file no longer exists then it is a DELETED event, otherwise it
        // must be a MODIFIED event
        const auto event_type = std::experimental::filesystem::exists(path)
            ? eng::file_event_type::MODIFIED
            : eng::file_event_type::DELETED;

        events->emplace_back(event_type);
    }
}

}

namespace eng
{

file_watcher_implementation::file_watcher_implementation(
    const std::experimental::filesystem::path &path,
    std::vector<file_event_type> * const events)
    : stream_(nullptr)
{
    // construct a CFString from the supplied path
    auto cf_path = cf_ptr(::CFStringCreateWithCString(
        nullptr,
        path.c_str(),
        kCFStringEncodingUTF8));

    // get a void pointer to the CFString, so we can add it to a CFArrray
    const auto *array_values = static_cast<const void*>(cf_path.get());

    // create CFArrray with our path
    const auto paths = cf_ptr(::CFArrayCreate(
        nullptr,
        static_cast<const void**>(&array_values),
        1,
        nullptr));

    // pass a pointer to the events collection as the callback user info
    FSEventStreamContext context{
        0,
        static_cast<void*>(events),
        nullptr,
        nullptr,
        nullptr
    };

    // construct stream
    stream_ = FSEventStreamCreate(
        nullptr,
        &callback,
        &context,
        paths.get(),
        kFSEventStreamEventIdSinceNow,
        0,
        kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagMarkSelf);

    // schedule stream on current run loop
    ::FSEventStreamScheduleWithRunLoop(
        stream_,
        CFRunLoopGetCurrent(),
        kCFRunLoopDefaultMode);

    ::FSEventStreamStart(stream_);
}

file_watcher_implementation::~file_watcher_implementation()
{
    ::FSEventStreamStop(stream_);
    ::FSEventStreamInvalidate(stream_);
    ::FSEventStreamRelease(stream_);
}

file_watcher_implementation::file_watcher_implementation(file_watcher_implementation &&other)
    : stream_(nullptr)
{
    swap(other);
}

file_watcher_implementation& file_watcher_implementation::operator=(file_watcher_implementation &&other)
{
    // create a new implementation object to 'steal' the internal state of the
    // supplied object
    file_watcher_implementation new_impl{ std::move(other) };
    swap(new_impl);

    return *this;
}

void file_watcher_implementation::swap(file_watcher_implementation &other)
{
    std::swap(stream_, other.stream_);
}

}

