#pragma once

#include <memory>

namespace iris
{

class JobSystem;
class Logger;
class PhysicsSystem;
class RenderSystem;
class Window;
class RenderTarget;

/**
 * A singleton object that provides access to various parts of the engine. It
 * controls the construction and destruction of components.
 *
 * This class should provide singleton access to all its components. This means
 * that they themselves do not have to be singletons.
 */
class Root
{
  public:
    ~Root() = default;

    /**
     * Initialises the Root object. Some platforms require deferred
     * initialisation. This *should not* be called directly, the engine
     * will call it when it safe to do so.
     */
    void init();

    /**
     * Get the single instance of root.
     *
     * @returns
     *   Root single instance.
     */
    static Root &instance();

    /**
     * Get single instance of job system.
     *
     * @returns
     *   Job system single instance.
     */
    static JobSystem &job_system();

    /**
     * Get single instance of logger.
     *
     * @returns
     *   Logger single instance.
     */
    static Logger &logger();

    /**
     * Get single instance of physics system.
     *
     * @returns
     *   Render system single instance.
     */
    static PhysicsSystem &physics_system();

    /**
     * Get single instance of render system.
     *
     * @returns
     *   Render system single instance.
     */
    static RenderSystem &render_system();

    /**
     * Get single instance of render window.
     *
     * @returns
     *   Render system single instance.
     */
    static Window &window();

    /**
     * Get single instance of screen render target. Anything rendered to this
     * target will end up on the screen.
     *
     * @returns
     *   Screen target single instance.
     */
    static RenderTarget &screen_target();

  private:
    /**
     * Private to force access via instance.
     */
    Root();

    /** Single instance. */
    static Root instance_;

    // *NOTE*
    // the order of the members is critical as we need to ensure destruction
    // happens in a fixed order due to dependencies between components
    // e.g. Logger must be destroyed last as other destructors use it

    /** Logger. */
    std::unique_ptr<Logger> logger_;

    /** Job system. */
    std::unique_ptr<JobSystem> job_system_;

    /** Physics system. */
    std::unique_ptr<PhysicsSystem> physics_system_;

    /** Render window. */
    std::unique_ptr<Window> window_;

    /** Render system. */
    std::unique_ptr<RenderSystem> render_system_;

    /** Screen target. */
    std::unique_ptr<RenderTarget> screen_target_;
};

}
