////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "core/start.h"

#include <memory>

#include "core/root.h"
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

void register_apis()
{
    iris::Root::register_graphics_api(
        "metal",
        std::make_unique<iris::MacosWindowManager>(),
        std::make_unique<iris::MetalMeshManager>(),
        std::make_unique<iris::MetalTextureManager>(),
        std::make_unique<iris::MetalMaterialManager>(),
        std::make_unique<iris::MetalRenderTargetManager>());
    iris::Root::set_graphics_api("metal");

    iris::Root::register_physics_api("bullet", std::make_unique<iris::BulletPhysicsManager>());
    iris::Root::set_physics_api("bullet");

#if defined(IRIS_ARCH_X86_64)
    iris::Root::register_jobs_api("thread", std::make_unique<iris::ThreadJobSystemManager>());
    iris::Root::register_jobs_api("fiber", std::make_unique<iris::FiberJobSystemManager>());
    iris::Root::set_jobs_api("fiber");
#else
    iris::Root::register_jobs_api("thread", std::make_unique<iris::ThreadJobSystemManager>());
    iris::Root::set_jobs_api("thread");
#endif
}

}

namespace iris
{

void start(int argc, char **argv, std::function<void(int, char **)> entry)
{
    Logger::instance().set_Formatter<EmojiFormatter>();

    LOG_ERROR("start", "engine start {}", IRIS_VERSION_STR);

    register_apis();

    entry(argc, argv);

    Root::reset();
}

void start_debug(int argc, char **argv, std::function<void(int, char **)> entry)
{
    // enable engine logging
    Logger::instance().set_Formatter<EmojiFormatter>();
    Logger::instance().set_log_engine(true);

    LOG_ENGINE_INFO("start", "engine start (with debugging) {}", IRIS_VERSION_STR);

    Profiler profiler{};

    register_apis();

    entry(argc, argv);

    Root::reset();
}

}
