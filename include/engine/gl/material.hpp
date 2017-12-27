#pragma once

#include <cstdint>

#include "gl/shader.hpp"

namespace eng::gl
{

/**
 * Class encapsulating an opengl material. Internally this creates and manages
 * an opengl program object.
 */
class material final
{
    public:

        /**
         * Construct a new material.
         *
         * @param vertex_shader
         *   Shader for vertex.
         *
         * @param fragment_shader
         *   Shader for fragment.
         */
        material(const shader &vertex_shader, const shader &fragment_shader);

        /**
         * Destructor, performs opengl cleanup.
         */
        ~material();

        /**
         * Move constructor, steals the state from the moved-in object.
         *
         * @param other
         *   Object to take state from. Do not use after this call.
         */
        material(material &&other) noexcept;

        /**
         * Move operator, steals the state from the moved-in object.
         *
         * @param other
         *   Object to take state from. Do not use after this call.
         */
        material& operator=(material &&) noexcept;

        /** Disabled */
        material(const material&) = delete;
        material& operator=(const material&) = delete;

        /**
         * Get the native opengl handle.
         *
         * @returns native opengl handle.
         */
        std::uint32_t native_handle() const noexcept;

        /**
         * Bind this material, all future draw calls will be rendered with
         * this material.
         */
        void bind() const;

        /**
         * Unbind this material.
         */
        void unbind() const;

    private:

        /** Opengl program object. */
        std::uint32_t program_;
};

}

