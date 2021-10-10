////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/start.h"

#include <memory>

#include "core/root.h"
#include "graphics/d3d12/d3d12_mesh_manager.h"
#include "graphics/d3d12/d3d12_texture_manager.h"
#include "graphics/opengl/opengl_mesh_manager.h"
#include "graphics/opengl/opengl_texture_manager.h"
#include "graphics/win32/win32_window_manager.h"
#include "iris_version.h"
#include "jobs/fiber/fiber_job_system_manager.h"
#include "jobs/thread/thread_job_system_manager.h"
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

    iris::Root::register_jobs_api(
        "thread", std::make_unique<iris::ThreadJobSystemManager>());

    iris::Root::register_jobs_api(
        "fiber", std::make_unique<iris::FiberJobSystemManager>());

    iris::Root::set_jobs_api("fiber");
}

}

namespace iris
{

void start(int argc, char **argv, std::function<void(int, char **)> entry)
{
    LOG_ENGINE_INFO("start", "engine start {}", IRIS_VERSION_STR);

    register_apis();

    entry(argc, argv);

    Root::reset();
}

void start_debug(int argc, char **argv, std::function<void(int, char **)> entry)
{
    // enable engine logging
    Logger::instance().set_log_engine(true);

    LOG_ENGINE_INFO(
        "start", "engine start (with debugging) {}", IRIS_VERSION_STR);

    register_apis();

    entry(argc, argv);

    Root::reset();
}

}
