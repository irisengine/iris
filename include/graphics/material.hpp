#pragma once

#include <any>
#include <memory>
#include <string>

namespace eng
{

/**
 * Class encapsulating a materal for rendering an entity.
 */
class material
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

        /** Declared in mm/cpp file as implementation is an incomplete file. */
        ~material();
        material(material&&);
        material& operator=(material&&);

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

