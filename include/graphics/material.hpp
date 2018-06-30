#pragma once

#include <any>
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

        /**
         * Get a native handle for the material. The type of this is dependant
         * on the current graphics API.
         *
         * @returns
         *   Graphics API specific handle.
         */
        std::any native_handle() const noexcept;

    private:

        /** Graphics API implementation. */
        material_implementation impl_;
};

}

