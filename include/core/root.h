#pragma once

#include "core/real.h"
#include "graphics/render_system.h"
#include "jobs/job_system.h"
#include "log/logger.h"
#include "platform/window.h"

namespace eng
{

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

        /**
         * Destructor. Ensure objects are unloaded in the correct order.
         */
        ~Root();

        /**
         * Get the single instance of root.
         *
         * @returns
         *   Root single instance.
         */
        static Root& instance();

        /**
         * Get single instance of job system.
         *
         * @returns
         *   Job system single instance.
         */
        static JobSystem& job_system();

        /**
         * Get single instance of logger.
         *
         * @returns
         *   Logger single instance.
         */
        static Logger& logger();

        /**
         * Get single instance of render system.
         *
         * @returns
         *   Render system single instance.
         */
        static RenderSystem& render_system();

        /**
         * Get single instance of render window.
         *
         * @returns
         *   Render system single instance.
         */
        static Window& window();

    private:

        /**
         * Private to force access via instance.
         */
        Root();

        /** Single instance. */
        static Root instance_;

        /** Logger. */
        Logger *logger_;

        /** Job system. */
        JobSystem *job_system_;

        /** Render window. */
        Window *window_;

        /** Render system. */
        RenderSystem *render_system_;
};

}

