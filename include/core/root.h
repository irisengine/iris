#pragma once

namespace eng
{

class JobSystem;
class Logger;

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
};

}

