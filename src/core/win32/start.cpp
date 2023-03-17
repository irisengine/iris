////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/start.h"

#include <algorithm>
#include <memory>

#include "core/context.h"
#include "core/default_resource_manager.h"
#include "core/error_handling.h"
#include "core/profiler.h"
#include "graphics/d3d12/d3d12_material_manager.h"
#include "graphics/d3d12/d3d12_mesh_manager.h"
#include "graphics/d3d12/d3d12_render_target_manager.h"
#include "graphics/d3d12/d3d12_texture_manager.h"
#include "graphics/opengl/opengl_material_manager.h"
#include "graphics/opengl/opengl_mesh_manager.h"
#include "graphics/opengl/opengl_render_target_manager.h"
#include "graphics/opengl/opengl_texture_manager.h"
#include "graphics/win32/win32_window_manager.h"
#include "iris_version.h"
#include "jobs/fiber/fiber_job_system_manager.h"
#include "jobs/thread/thread_job_system_manager.h"
#include "log/log.h"
#include "log/logger.h"
#include "physics/bullet/bullet_physics_manager.h"

#include <objbase.h>

namespace
{

/**
 * Create the engine context object, with defaults for the current platform.
 *
 * @param argc
 *   Number of command line arguments.
 *
 * @param argv
 *   Array of command line arguments.
 *
 * @returns
 *   Engine conetext object.
 */
iris::Context create_context(int argc, char **argv)
{
    iris::Context ctx{argc, argv};

    auto resource_manager = std::make_unique<iris::DefaultResourceManager>();

    auto d3d12_texture_manager = std::make_unique<iris::D3D12TextureManager>(*resource_manager);
    auto d3d12_material_manager = std::make_unique<iris::D3D12MaterialManager>();
    auto d3d12_window_manager =
        std::make_unique<iris::Win32WindowManager>(*d3d12_texture_manager, *d3d12_material_manager, "d3d12");
    auto d3d12_mesh_manager = std::make_unique<iris::D3D12MeshManager>(*resource_manager);
    auto d3d12_render_target_manager =
        std::make_unique<iris::D3D12RenderTargetManager>(*d3d12_window_manager, *d3d12_texture_manager);

    ctx.register_graphics_api(
        "d3d12",
        std::move(d3d12_window_manager),
        std::move(d3d12_mesh_manager),
        std::move(d3d12_texture_manager),
        std::move(d3d12_material_manager),
        std::move(d3d12_render_target_manager));

    auto opengl_texture_manager = std::make_unique<iris::OpenGLTextureManager>(*resource_manager);
    auto opengl_material_manager = std::make_unique<iris::OpenGLMaterialManager>();
    auto opengl_window_manager =
        std::make_unique<iris::Win32WindowManager>(*opengl_texture_manager, *opengl_material_manager, "opengl");
    auto opengl_mesh_manager = std::make_unique<iris::OpenGLMeshManager>(*resource_manager);
    auto opengl_render_target_manager =
        std::make_unique<iris::OpenGLRenderTargetManager>(*opengl_window_manager, *opengl_texture_manager);

    ctx.register_graphics_api(
        "opengl",
        std::move(opengl_window_manager),
        std::move(opengl_mesh_manager),
        std::move(opengl_texture_manager),
        std::move(opengl_material_manager),
        std::move(opengl_render_target_manager));

    ctx.set_graphics_api("d3d12");

    ctx.register_physics_api("bullet", std::make_unique<iris::BulletPhysicsManager>(ctx.mesh_manager()));
    ctx.set_physics_api("bullet");

    ctx.register_jobs_api("thread", std::make_unique<iris::ThreadJobSystemManager>());
    ctx.register_jobs_api("fiber", std::make_unique<iris::FiberJobSystemManager>());
    ctx.set_jobs_api("fiber");

    ctx.set_resource_manager(std::move(resource_manager));

    return ctx;
}

}

namespace iris
{

void start(int argc, char **argv, std::function<void(Context)> entry, bool debug)
{
    std::unique_ptr<Profiler> profiler;

    if (debug)
    {
        profiler = std::make_unique<Profiler>();

        Logger::instance().set_log_engine(true);
        LOG_ENGINE_INFO("start", "debug mode on");
    }

    LOG_ENGINE_INFO("start", "engine start {}", IRIS_VERSION_STR);

    ensure(::CoInitializeEx(nullptr, COINIT_MULTITHREADED) == S_OK, "CoInitialize failed");

    entry(create_context(argc, argv));
}

}
