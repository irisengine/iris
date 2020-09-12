#pragma once

#include <any>
#include <memory>
#include <string>

namespace iris
{

/**
 * Class encapsulating a materal for rendering an entity.
 */
class Material
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
        Material(
            const std::string &vertex_shader_source,
            const std::string &fragment_shader_source);

        /** Declared in mm/cpp file as implementation is an incomplete file. */
        ~Material();
        Material(Material&&);
        Material& operator=(Material&&);

        /**
         * Get a native handle for the material. The type of this is dependant
         * on the current graphics API.
         *
         * @returns
         *   Graphics API specific handle.
         */
        std::any native_handle() const;

    private:

        /** Graphics API implementation. */
        struct implementation;
        std::unique_ptr<implementation> impl_;
};

}

