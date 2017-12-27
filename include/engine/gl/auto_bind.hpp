#pragma once

namespace eng::gl
{

/**
 * Helper RAII-style class to bind an object on construction and unbind on
 * destruction.
 *
 * @typeparam T
 *   An object with bind/unbind methods.
 */
template<class T>
class auto_bind final
{
    public:

        /**
         * Construct a new auto_bind object and call bind on the supplied
         * object.
         *
         * @param object
         *   Object to auto bind and unbind.
         */
        explicit auto_bind(const T &object)
            : object_(object)
        {
            object_.bind();
        }

        /**
         * Destructor, call unbind on supplied object.
         */
        ~auto_bind()
        {
            object_.unbind();
        }

    private:

        /** Reference to object to bind/unbind. */
        const T &object_;
};

}

