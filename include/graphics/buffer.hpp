#pragma once

#include <any>
#include <cstdint>
#include <memory>

#include "buffer_type.hpp"
#include "vertex_data.hpp"

namespace eng
{

/**
 * Class encapsulating a graphics api specific data buffer.
 */
class buffer final
{
    public:

        /**
         * Construct a new buffer and copy supplied float data.
         *
         * @param data
         *   Data to store in buffer.
         */
        buffer(const std::vector<float> &data, const buffer_type type);

        /**
         * Construct a new buffer and copy supplied uint32_t data.
         *
         * @param data
         *   Data to store in buffer.
         */
        buffer(const std::vector<std::uint32_t> &data, const buffer_type type);

        /**
         * Construct a new buffer and copy supplied vertex_data data.
         *
         * @param data
         *   Data to store in buffer.
         */
        buffer(const std::vector<vertex_data> &data, const buffer_type type);

        /** Declared in mm/cpp file as implementation is an incomplete type. */
        ~buffer();
        buffer(buffer &&other);
        buffer& operator=(buffer &&other);

        /**
         * Get native handle for buffer.
         *
         * @returns
         *   Buffer native handle.
         */
        std::any native_handle() const;

        /**
         * Type of buffer.
         *
         * @returns
         *   Buffer type.
         */
        buffer_type type() const noexcept;

    private:

        /** Graphics API implementation. */
        struct implementation;
        std::unique_ptr<implementation> impl_;

        /** Buffer type. */
        buffer_type type_;
};

}

