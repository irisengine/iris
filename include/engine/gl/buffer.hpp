#pragma once

#include <cstdint>
#include <vector>

#include "gl/auto_bind.hpp"
#include "gl/opengl.hpp"

namespace eng::gl
{

/**
 * Class encapsulating an opengl data buffer.
 */
class buffer final
{
    public:

        /**
         * Construct a new buffer and copy supplied data.
         *
         * @typeparam T
         *   Type of data to opy.
         *
         * @param data
         *   Data to copy.
         */
        template<class T>
        buffer(const std::vector<T> &data, const std::uint32_t type)
            : handle_(0u),
              type_(type)
        {
            ::glGenBuffers(1, &handle_);
            gl::check_opengl_error("could not generate opengl buffer");

            // bind so we can copy data
            auto_bind<decltype(*this)> auto_data{ *this };

            ::glBufferData(
                type_,
                data.size() * sizeof(T),
                data.data(),
                GL_STATIC_DRAW);
            gl::check_opengl_error("could not buffer data");
        }

        /**
         * Destructor, performs opengl cleanup.
         */
        ~buffer();

        /**
         * Move constructor, steals the state from the moved-in object.
         *
         * @param other
         *   Object to take state from. Do not use after this call.
         */
        buffer(buffer &&other) noexcept;

        /**
         * Move operator, steals the state from the moved-in object.
         *
         * @param other
         *   Object to take state from. Do not use after this call.
         */
        buffer& operator=(buffer &&other) noexcept;

        /** Disabled */
        buffer(const buffer&) = delete;
        buffer& operator=(const buffer&) = delete;;

        /**
         * Bind this data.
         */
        void bind() const;

        /**
         * Unbind this data.
         */
        void unbind() const;

    private:

        /** Opengl buffer handle. */
        std::uint32_t handle_;

        /** Type of buffer object. */
        std::uint32_t type_;
};

}

