#pragma once

#include <cstdint>

namespace eng
{

/**
 * Class which stores opengl rendering state.
 *
 * Internally this uses a VAO object, any changes made after bind are tracked
 * until unbind is called.
 */
class vertex_state final
{
    public:

        /**
         * Construct a new vertex_state.
         */
        vertex_state();

        /**
         * Destructor, performs opengl cleanup.
         */
        ~vertex_state();

        /**
         * Move constructor, steals the state from the moved-in object.
         *
         * @param other
         *   Object to take state from. Do not use after this call.
         */
        vertex_state(vertex_state &&other) noexcept;

        /**
         * Move operator, steals the state from the moved-in object.
         *
         * @param other
         *   Object to take state from. Do not use after this call.
         */
        vertex_state& operator=(vertex_state &&other) noexcept;

        /** Disabled */
        vertex_state(const vertex_state&) = delete;
        vertex_state& operator=(const vertex_state&) = delete;

        /**
         * Get the native opengl handle.
         *
         * @returns native opengl handle.
         */
        std::uint32_t native_handle() const noexcept;

        /**
         * Bind this state. Any changes to the opengl state will be tracked
         * and any rendering calls will use the state bound to this object.
         */
        void bind() const;

        /**
         * Unbind this state.
         */
        void unbind() const;

    private:

        /** Opengl VAO object. */
        std::uint32_t vao_;
};

}

