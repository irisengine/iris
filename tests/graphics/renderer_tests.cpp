#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "fakes/fake_renderer.h"
#include "graphics/render_command.h"
#include "graphics/render_command_type.h"

TEST(renderer_test, command_execution)
{
    const std::vector<iris::RenderCommandType> expected{
        iris::RenderCommandType::UPLOAD_TEXTURE,
        iris::RenderCommandType::PASS_START,
        iris::RenderCommandType::DRAW,
        iris::RenderCommandType::PASS_END,
        iris::RenderCommandType::PRESENT};

    std::vector<iris::RenderCommand> render_queue{};

    for (const auto &command_type : expected)
    {
        iris::RenderCommand cmd;
        cmd.set_type(command_type);
        render_queue.emplace_back(cmd);
    }

    FakeRenderer renderer{render_queue};

    renderer.render();

    ASSERT_EQ(renderer.call_log(), expected);
}
