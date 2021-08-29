#include "core/root.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "core/exception.h"
#include "graphics/mesh_manager.h"
#include "graphics/texture_manager.h"
#include "graphics/window_manager.h"

namespace iris
{

Root::Root()
    : graphics_api_managers_()
    , graphics_api_()
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

}
