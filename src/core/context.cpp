////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/context.h"

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "core/error_handling.h"
#include "core/resource_manager.h"
#include "graphics/material_manager.h"
#include "graphics/mesh_manager.h"
#include "graphics/render_target_manager.h"
#include "graphics/texture_manager.h"
#include "graphics/window_manager.h"
#include "jobs/job_system_manager.h"
#include "physics/physics_manager.h"

namespace iris
{

Context::Context(int argc, char **argv)
    : graphics_api_managers_()
    , graphics_api_()
    , physics_api_managers_()
    , physics_api_()
    , jobs_api_managers_()
    , jobs_api_()
    , resource_manager_()
    , args_(argc)
{
    std::transform(argv, argv + argc, std::begin(args_), [](const char *arg) -> std::string { return arg; });
}

Context::~Context()
{
    graphics_api_managers_.clear();
    physics_api_managers_.clear();
    jobs_api_managers_.clear();
}

Context::Context(Context &&) = default;
Context &Context::operator=(Context &&) = default;

const std::vector<std::string> &Context::args() const
{
    return args_;
}
WindowManager &Context::window_manager() const
{
    return *graphics_api_managers_.at(graphics_api_).window_manager;
}

MeshManager &Context::mesh_manager() const
{
    return *graphics_api_managers_.at(graphics_api_).mesh_manager;
}

TextureManager &Context::texture_manager() const
{
    return *graphics_api_managers_.at(graphics_api_).texture_manager;
}

MaterialManager &Context::material_manager() const
{
    return *graphics_api_managers_.at(graphics_api_).material_manager;
}

RenderTargetManager &Context::render_target_manager() const
{
    return *graphics_api_managers_.at(graphics_api_).render_target_manager;
}

PhysicsManager &Context::physics_manager() const
{
    return *physics_api_managers_.at(physics_api_);
}

JobSystemManager &Context::jobs_manager() const
{
    return *jobs_api_managers_.at(jobs_api_);
}

ResourceManager &Context::resource_manager() const
{
    return *resource_manager_;
}

void Context::register_graphics_api(
    const std::string &api,
    std::unique_ptr<WindowManager> window_manager,
    std::unique_ptr<MeshManager> mesh_manager,
    std::unique_ptr<TextureManager> texture_manager,
    std::unique_ptr<MaterialManager> material_manager,
    std::unique_ptr<RenderTargetManager> render_target_manager)
{
    GraphicsApiManagers managers{
        .mesh_manager = std::move(mesh_manager),
        .texture_manager = std::move(texture_manager),
        .material_manager = std::move(material_manager),
        .render_target_manager = std::move(render_target_manager),
        .window_manager = std::move(window_manager)};

    graphics_api_managers_[api] = std::move(managers);
}

std::string Context::graphics_api() const
{
    return graphics_api_;
}

void Context::set_graphics_api(const std::string &api)
{
    ensure(graphics_api_managers_.count(api) != 0u, "api not registered");

    graphics_api_ = api;
}

std::vector<std::string> Context::registered_graphics_apis() const
{
    std::vector<std::string> apis{};

    for (const auto &[api, _] : graphics_api_managers_)
    {
        apis.emplace_back(api);
    }

    return apis;
}

void Context::register_physics_api(const std::string &api, std::unique_ptr<PhysicsManager> physics_manager)
{
    physics_api_managers_[api] = std::move(physics_manager);
}

std::string Context::physics_api() const
{
    return physics_api_;
}

void Context::set_physics_api(const std::string &api)
{
    ensure(physics_api_managers_.count(api) != 0u, "api not registered");

    physics_api_ = api;
}

std::vector<std::string> Context::registered_physics_apis() const
{
    std::vector<std::string> apis{};

    for (const auto &[api, _] : physics_api_managers_)
    {
        apis.emplace_back(api);
    }

    return apis;
}

void Context::register_jobs_api(const std::string &api, std::unique_ptr<JobSystemManager> jobs_manager)
{
    jobs_api_managers_[api] = std::move(jobs_manager);
}

std::string Context::jobs_api() const
{
    return jobs_api_;
}

void Context::set_jobs_api(const std::string &api)
{
    ensure(jobs_api_managers_.count(api) != 0u, "api not registered");

    jobs_api_ = api;

    jobs_manager().create_job_system();
}

std::vector<std::string> Context::registered_jobs_apis() const
{
    std::vector<std::string> apis{};

    for (const auto &[api, _] : jobs_api_managers_)
    {
        apis.emplace_back(api);
    }

    return apis;
}

void Context::set_resource_manager(std::unique_ptr<ResourceManager> resource_manager)
{
    resource_manager_ = std::move(resource_manager);
}

}
