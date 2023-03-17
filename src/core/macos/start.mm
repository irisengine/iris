////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/start.h"

#include <memory>

#include "core/context.h"
#include "core/default_resource_manager.h"
#include "core/profiler.h"
#include "graphics/macos/macos_window_manager.h"
#include "graphics/metal/metal_material_manager.h"
#include "graphics/metal/metal_mesh_manager.h"
#include "graphics/metal/metal_render_target_manager.h"
#include "graphics/metal/metal_texture_manager.h"
#include "iris_version.h"
#include "jobs/thread/thread_job_system_manager.h"
#include "log/emoji_formatter.h"
#include "log/log.h"
#include "log/logger.h"
#include "physics/bullet/bullet_physics_manager.h"

#if defined(IRIS_ARCH_X86_64)
#include "jobs/fiber/fiber_job_system_manager.h"
#endif

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

    auto metal_texture_manager = std::make_unique<iris::MetalTextureManager>(*resource_manager);
    auto metal_material_manager = std::make_unique<iris::MetalMaterialManager>();
    auto metal_window_manager =
        std::make_unique<iris::MacosWindowManager>(*metal_texture_manager, *metal_material_manager);
    auto metal_mesh_manager = std::make_unique<iris::MetalMeshManager>(*resource_manager);
    auto metal_render_target_manager =
        std::make_unique<iris::MetalRenderTargetManager>(*metal_window_manager, *metal_texture_manager);

    ctx.register_graphics_api(
        "metal",
        std::move(metal_window_manager),
        std::move(metal_mesh_manager),
        std::move(metal_texture_manager),
        std::move(metal_material_manager),
        std::move(metal_render_target_manager));

    ctx.set_graphics_api("metal");

    ctx.register_physics_api("bullet", std::make_unique<iris::BulletPhysicsManager>(ctx.mesh_manager()));
    ctx.set_physics_api("bullet");

#if defined(IRIS_ARCH_X86_64)
    ctx.register_jobs_api("thread", std::make_unique<iris::ThreadJobSystemManager>());
    ctx.register_jobs_api("fiber", std::make_unique<iris::FiberJobSystemManager>());
    ctx.set_jobs_api("fiber");
#else
    ctx.register_jobs_api("thread", std::make_unique<iris::ThreadJobSystemManager>());
    ctx.set_jobs_api("thread");
#endif

    ctx.set_resource_manager(std::move(resource_manager));

    return ctx;
}

}

namespace iris
{

void start(int argc, char **argv, std::function<void(Context)> entry, bool debug)
{
    Logger::instance().set_Formatter<EmojiFormatter>();

    std::unique_ptr<Profiler> profiler;

    if (debug)
    {
        profiler = std::make_unique<Profiler>();

        Logger::instance().set_log_engine(true);
        LOG_ENGINE_INFO("start", "debug mode on");
    }

    LOG_ENGINE_INFO("start", "engine start {}", IRIS_VERSION_STR);

    entry(create_context(argc, argv));
}

}
