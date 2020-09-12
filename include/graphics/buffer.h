#pragma once

#include <any>
#include <cstdint>
#include <memory>

#include "buffer_type.h"
#include "vertex_data.h"

namespace iris
{

/**
 * Class encapsulating a graphics api specific data buffer.
 */
class Buffer
{
    public:

        /**
         * Construct a new Buffer and copy supplied float data.
         *
         * @param data
         *   Data to store in buffer.
         */
        Buffer(const std::vector<float> &data, const BufferType type);

        /**
         * Construct a new Buffer and copy supplied uint32_t data.
         *
         * @param data
         *   Data to store in buffer.
         */
        Buffer(const std::vector<std::uint32_t> &data, const BufferType type);

        /**
         * Construct a new Buffer and copy supplied vertex_data data.
         *
         * @param data
         *   Data to store in buffer.
         */
        Buffer(const std::vector<vertex_data> &data, const BufferType type);

        /** Declared in mm/cpp file as implementation is an incomplete type. */
        ~Buffer();
        Buffer(Buffer &&other);
        Buffer& operator=(Buffer &&other);

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
        BufferType type() const;

    private:

        /** Graphics API implementation. */
        struct implementation;
        std::unique_ptr<implementation> impl_;

        /** Buffer type. */
        BufferType type_;
};

}

