#pragma once

#include <cstdint>
#include <string>

namespace eng::gl
{

/**
 * This class is the opengl implementation of material. This encapsulates all the
 * logic needed for an opengl material.
 */
class material_implementation final
{
    public:

        /**
         * Create a new material_implementation.
         *
         * @param vertex_shader_source
         *   The source for a vertex shader.
         *
         * @param fragment_shader_source
         *   The source for a fragment shader.
         */
        material_implementation(
            const std::string &vertex_shader_source,
            const std::string &fragment_shader_source);

        /**
         * Perform opengl cleanup.
         */
        ~material_implementation();

        /**
         * Move constructor, steals the state from the moved-in object.
         *
         * @param other
         *   Object to take state from. Do not use after this call.
         */
        material_implementation(material_implementation &&other) noexcept;

        /**
         * Move operator, steals the state from the moved-in object.
         *
         * @param other
         *   Object to take state from. Do not use after this call.
         */
        material_implementation& operator=(material_implementation &&other) noexcept;

        /** Disabled */
        material_implementation(const material_implementation&) = delete;
        material_implementation& operator=(const material_implementation&) = delete;

        /**
         * Performs all opengl actions required to use this material for
         * rendering with.
         */
        void bind() const;

        /**
         * Performs all opengl actions needed after rendering with this
         * material.
         */
        void unbind() const;

        /**
         * Get the native handle for the opengl program.
         *
         * @returns
         *   Opengl program handle.
         */
        std::uint32_t native_handle() const noexcept;

    private:

        /** Opengl program. */
        std::uint32_t program_;
};

}

