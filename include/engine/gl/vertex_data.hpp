#pragma once

#include <cstdint>
#include <vector>

#include "gl/auto_bind.hpp"
#include "gl/opengl.hpp"

namespace eng::gl
{

/**
 * Class encapsulating an opengl vertex data buffer.
 */
class vertex_data final
{
    public:

        /**
         * Construct a new vertex_data and buffer supplied data.
         *
         * @typeparam T
         *   Type of data to buffer.
         *
         * @param data
         *   Data to buffer.
         */
        template<class T>
        explicit vertex_data(const std::vector<T> &data)
            : vbo_(0u)
        {
            ::glGenBuffers(1, &vbo_);
            gl::check_opengl_error("could not generate vbo");

            auto_bind<decltype(*this)> auto_data{ *this };

            ::glBufferData(
                GL_ARRAY_BUFFER,
                data.size() * sizeof(T),
                data.data(),
                GL_STATIC_DRAW);
            gl::check_opengl_error("could not buffer data");
        }

        /**
         * Destructor, performs opengl cleanup.
         */
        ~vertex_data();

        /**
         * Move constructor, steals the state from the moved-in object.
         *
         * @param other
         *   Object to take state from. Do not use after this call.
         */
        vertex_data(vertex_data &&other) noexcept;

        /**
         * Move operator, steals the state from the moved-in object.
         *
         * @param other
         *   Object to take state from. Do not use after this call.
         */
        vertex_data& operator=(vertex_data &&other) noexcept;

        /** Disabled */
        vertex_data(const vertex_data&) = delete;
        vertex_data& operator=(const vertex_data&) = delete;;

        /**
         * Bind this data.
         */
        void bind() const;

        /**
         * Unbind this data.
         */
        void unbind() const;

    private:

        /** Opengl VBO object. */
        std::uint32_t vbo_;
};

}

