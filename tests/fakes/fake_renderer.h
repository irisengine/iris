#pragma once

#include <cstdint>
#include <vector>

#include "graphics/render_command.h"
#include "graphics/render_command_type.h"
#include "graphics/render_pass.h"
#include "graphics/render_target.h"
#include "graphics/renderer.h"

class FakeRenderer : public iris::Renderer
{
  public:
    FakeRenderer(const std::vector<iris::RenderCommand> &render_queue)
        : iris::Renderer()
    {
        render_queue_ = render_queue;
    }

    std::vector<iris::RenderCommandType> call_log() const
    {
        return call_log_;
    }

    ~FakeRenderer() override = default;

    // overridden methods which just log when they are called

    void set_render_passes(const std::vector<iris::RenderPass> &) override
    {
    }

    iris::RenderTarget *create_render_target(std::uint32_t, std::uint32_t)
        override
    {
        return nullptr;
    }

    void pre_render() override
    {
    }

    void execute_upload_texture(iris::RenderCommand &) override
    {
        call_log_.emplace_back(iris::RenderCommandType::UPLOAD_TEXTURE);
    }

    void execute_pass_start(iris::RenderCommand &) override
    {
        call_log_.emplace_back(iris::RenderCommandType::PASS_START);
    }

    void execute_draw(iris::RenderCommand &) override
    {
        call_log_.emplace_back(iris::RenderCommandType::DRAW);
    }

    void execute_pass_end(iris::RenderCommand &) override
    {
        call_log_.emplace_back(iris::RenderCommandType::PASS_END);
    }

    void execute_present(iris::RenderCommand &) override
    {
        call_log_.emplace_back(iris::RenderCommandType::PRESENT);
    }

    void post_render() override
    {
    }

  private:
    std::vector<iris::RenderCommandType> call_log_;
};