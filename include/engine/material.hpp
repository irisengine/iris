#pragma once

#include <cstdint>
#include <string>

#include "gl/material_implementation.hpp"

namespace eng
{

/**
 * Class encapsulating a materal for rendering an entity.
 */
class material final
{
    public:

        /**
         * Construct a new material.
         *
         * @param vertex_shader_source
         *   Shader source for vertex.
         *
         * @param fragment_shader_source
         *   Shader source for fragment.
         */
        material(
            const std::string &vertex_shader_source,
            const std::string &fragment_shader_source);

        /** Default */
        ~material() = default;
        material(material&&) = default;
        material& operator=(material&&) = default;

        /** Disabled */
        material(const material&) = delete;
        material& operator=(const material&) = delete;

        template<class T>
        T native_handle() const noexcept
        {
            return impl_.native_handle();
        }

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

        /** Graphics API implementation. */
        gl::material_implementation impl_;
};

}

