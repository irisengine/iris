#include "core/start.h"

#include <memory>

#include "core/root.h"
#include "graphics/d3d12/d3d12_mesh_manager.h"
#include "graphics/d3d12/d3d12_texture_manager.h"
#include "graphics/opengl/opengl_mesh_manager.h"
#include "graphics/opengl/opengl_texture_manager.h"
#include "graphics/win32/win32_window_manager.h"
#include "iris_version.h"
#include "log/log.h"
#include "log/logger.h"
#include "physics/bullet/bullet_physics_manager.h"

namespace
{

void register_apis()
{
    iris::Root::register_graphics_api(
        "d3d12",
        std::make_unique<iris::Win32WindowManager>(),
        std::make_unique<iris::D3D12MeshManager>(),
        std::make_unique<iris::D3D12TextureManager>());

    iris::Root::register_graphics_api(
        "opengl",
        std::make_unique<iris::Win32WindowManager>(),
        std::make_unique<iris::OpenGLMeshManager>(),
        std::make_unique<iris::OpenGLTextureManager>());

    iris::Root::set_graphics_api("d3d12");

    iris::Root::register_physics_api(
        "bullet", std::make_unique<iris::BulletPhysicsManager>());

    iris::Root::set_physics_api("bullet");
}

}

namespace iris
{

void start(int argc, char **argv, std::function<void(int, char **)> entry)
{
    LOG_ENGINE_INFO("start", "engine start {}", IRIS_VERSION_STR);

    register_apis();

    entry(argc, argv);
}

void start_debug(int argc, char **argv, std::function<void(int, char **)> entry)
{
    // enable engine logging
    Logger::instance().set_log_engine(true);

    LOG_ENGINE_INFO(
        "start", "engine start (with debugging) {}", IRIS_VERSION_STR);

    register_apis();

    entry(argc, argv);
}

}
