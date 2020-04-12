#include <filesystem>
#include <fstream>
#include <memory>
#include <thread>
#include <cstdlib>

#include <uuid/uuid.h>

#include <gtest/gtest.h>

#include "exception.hpp"
#include "file_event_type.hpp"
#include "file_watcher.hpp"
#include "platform/osx/runloop_helper.hpp"

struct temp_file
{
    temp_file()
    {
        uuid_t id{ };
        ::uuid_generate(id);

        std::string filename(37, '\0');
        ::uuid_unparse(id, filename.data());

        path = std::filesystem::temp_directory_path() / filename;

        stream = std::fstream(path, std::ios::out);

        EXPECT_TRUE(stream.is_open());
        EXPECT_TRUE(std::filesystem::exists(path));
        EXPECT_TRUE(std::filesystem::is_regular_file(path));
    }

    void write([[maybe_unused]] const std::string &data)
    {
        stream << data;
        stream.flush();
        EXPECT_NE(0, std::filesystem::file_size(path));
    }

    ~temp_file()
    {
        stream.close();
        EXPECT_TRUE(std::filesystem::remove(path));
    }

    std::filesystem::path path;

    std::fstream stream;
};

TEST(file_watcher_tests, throw_if_no_file)
{
    ASSERT_THROW(eng::file_watcher{ "/not/a/file" }, eng::exception);
}

TEST(file_watcher_tests, construct)
{
    temp_file file{ };
    eng::file_watcher fw{ file.path };

    ASSERT_FALSE(fw.has_events());
    ASSERT_EQ(0, fw.yield().size());
}

TEST(file_watcher_tests, modified_event)
{
    temp_file file{ };
    eng::file_watcher fw{ file.path };

    file.write("hello");

    while(!fw.has_events())
    {
        run_runloop();
    }

    ASSERT_TRUE(fw.has_events());
    const auto events = fw.yield();
    ASSERT_FALSE(fw.has_events());

    ASSERT_EQ(1, events.size());
    ASSERT_EQ(eng::file_event_type::MODIFIED, events.front());
}

TEST(file_watcher_tests, deleted_event)
{
    std::unique_ptr<eng::file_watcher> fw{ nullptr };

    {
        temp_file file{ };
        fw = std::make_unique<eng::file_watcher>(file.path);
    }

    while(!fw->has_events())
    {
        run_runloop();
    }

    const auto events = fw->yield();
    ASSERT_FALSE(fw->has_events());

    ASSERT_EQ(1, events.size());
    ASSERT_EQ(eng::file_event_type::DELETED, events.front());
}

