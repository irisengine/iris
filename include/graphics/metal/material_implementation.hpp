#pragma once

#include <any>
#include <string>

namespace eng
{

/**
 * This class is the metal implementation of material. This encapsulates all
 * the logic needed for a metal material.
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

        /** Default */
        ~material_implementation() = default;
        material_implementation(material_implementation &&other) = default;
        material_implementation& operator=(material_implementation &&other) = default;

        /** Disabled */
        material_implementation(const material_implementation&) = delete;
        material_implementation& operator=(const material_implementation&) = delete;

        /**
         * Get the native handle for the metal pipeline state.
         *
         * @returns
         *   Metal pipeline state handle.
         */
        std::any native_handle() const noexcept;

    private:

        /** Native handle for metal pipeline state. */
        std::any pipeline_state_;
};

}

