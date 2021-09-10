#include "core/root.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "core/exception.h"
#include "graphics/mesh_manager.h"
#include "graphics/texture_manager.h"
#include "graphics/window_manager.h"
#include "jobs/job_system_manager.h"
#include "physics/physics_manager.h"

namespace iris
{

Root::Root()
    : graphics_api_managers_()
    , graphics_api_()
    , physics_api_managers_()
    , physics_api_()
    , jobs_api_managers_()
    , jobs_api_()
{
}

Root &Root::instance()
{
    static Root root{};
    return root;
}

WindowManager &Root::window_manager()
{
    return instance().window_manager_impl();
}

MeshManager &Root::mesh_manager()
{
    return instance().mesh_manager_impl();
}

TextureManager &Root::texture_manager()
{
    return instance().texture_manager_impl();
}

PhysicsManager &Root::physics_manager()
{
    return instance().physics_manager_impl();
}

JobSystemManager &Root::jobs_manager()
{
    return instance().jobs_manager_impl();
}

void Root::register_graphics_api(
    const std::string &api,
    std::unique_ptr<WindowManager> window_manager,
    std::unique_ptr<MeshManager> mesh_manager,
    std::unique_ptr<TextureManager> texture_manager)
{
    return instance().register_graphics_api_impl(
        api,
        std::move(window_manager),
        std::move(mesh_manager),
        std::move(texture_manager));
}

std::string Root::graphics_api()
{
    return instance().graphics_api_impl();
}

void Root::set_graphics_api(const std::string &api)
{
    return instance().set_graphics_api_impl(api);
}

std::vector<std::string> Root::registered_graphics_apis()
{
    return instance().registered_graphics_apis_impl();
}

void Root::register_physics_api(
    const std::string &api,
    std::unique_ptr<PhysicsManager> physics_manager)
{
    return instance().register_physics_api_impl(
        api, std::move(physics_manager));
}

std::string Root::physics_api()
{
    return instance().physics_api_impl();
}

void Root::set_physics_api(const std::string &api)
{
    return instance().set_physics_api_impl(api);
}

std::vector<std::string> Root::registered_physics_apis()
{
    return instance().registered_physics_apis_impl();
}

void Root::register_jobs_api(
    const std::string &api,
    std::unique_ptr<JobSystemManager> jobs_manager)
{
    return instance().register_jobs_api_impl(api, std::move(jobs_manager));
}

std::string Root::jobs_api()
{
    return instance().jobs_api_impl();
}

void Root::set_jobs_api(const std::string &api)
{
    return instance().set_jobs_api_impl(api);
}

std::vector<std::string> Root::registered_jobs_apis()
{
    return instance().registered_jobs_apis_impl();
}

WindowManager &Root::window_manager_impl() const
{
    return *graphics_api_managers_.at(graphics_api_).window_manager;
}

MeshManager &Root::mesh_manager_impl() const
{
    return *graphics_api_managers_.at(graphics_api_).mesh_manager;
}

TextureManager &Root::texture_manager_impl() const
{
    return *graphics_api_managers_.at(graphics_api_).texture_manager;
}

PhysicsManager &Root::physics_manager_impl() const
{
    return *physics_api_managers_.at(physics_api_);
}

JobSystemManager &Root::jobs_manager_impl() const
{
    return *jobs_api_managers_.at(jobs_api_);
}

void Root::register_graphics_api_impl(
    const std::string &api,
    std::unique_ptr<WindowManager> window_manager,
    std::unique_ptr<MeshManager> mesh_manager,
    std::unique_ptr<TextureManager> texture_manager)
{
    GraphicsApiManagers managers{};
    managers.window_manager = std::move(window_manager);
    managers.mesh_manager = std::move(mesh_manager);
    managers.texture_manager = std::move(texture_manager);

    graphics_api_managers_[api] = std::move(managers);
}

std::string Root::graphics_api_impl() const
{
    return graphics_api_;
}

void Root::set_graphics_api_impl(const std::string &api)
{
    if (graphics_api_managers_.count(api) == 0u)
    {
        throw Exception("api not registered");
    }

    graphics_api_ = api;
}

std::vector<std::string> Root::registered_graphics_apis_impl() const
{
    std::vector<std::string> apis{};

    for (const auto &[api, _] : graphics_api_managers_)
    {
        apis.emplace_back(api);
    }

    return apis;
}

void Root::register_physics_api_impl(
    const std::string &api,
    std::unique_ptr<PhysicsManager> physics_manager)
{
    physics_api_managers_[api] = std::move(physics_manager);
}

std::string Root::physics_api_impl() const
{
    return physics_api_;
}

void Root::set_physics_api_impl(const std::string &api)
{
    if (physics_api_managers_.count(api) == 0u)
    {
        throw Exception("api not registered");
    }

    physics_api_ = api;
}

std::vector<std::string> Root::registered_physics_apis_impl() const
{
    std::vector<std::string> apis{};

    for (const auto &[api, _] : physics_api_managers_)
    {
        apis.emplace_back(api);
    }

    return apis;
}

void Root::register_jobs_api_impl(
    const std::string &api,
    std::unique_ptr<JobSystemManager> jobs_manager)
{
    jobs_api_managers_[api] = std::move(jobs_manager);
}

std::string Root::jobs_api_impl() const
{
    return jobs_api_;
}

void Root::set_jobs_api_impl(const std::string &api)
{
    if (jobs_api_managers_.count(api) == 0u)
    {
        throw Exception("api not registered");
    }

    jobs_api_ = api;

    jobs_manager_impl().create_job_system();
}

std::vector<std::string> Root::registered_jobs_apis_impl() const
{
    std::vector<std::string> apis{};

    for (const auto &[api, _] : jobs_api_managers_)
    {
        apis.emplace_back(api);
    }

    return apis;
}

}
