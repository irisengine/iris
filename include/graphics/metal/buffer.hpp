#pragma once

#include <any>
#include <cstdint>

namespace eng
{

/**
 * Class encapsulating a metal data buffer.
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
        buffer(const std::vector<float> &data);

        /**
         * Construct a new buffer and copy supplied uint32_t data.
         *
         * @param data
         *   Data to store in buffer.
         */
        buffer(const std::vector<std::uint32_t> &data);

        /** Default */
        ~buffer() = default;
        buffer(buffer &&other) = default;
        buffer& operator=(buffer && other) = default;

        /** Disabled */
        buffer(const buffer&) = delete;
        buffer& operator=(const buffer&) = delete;

        /**
         * Get native handle for buffer.
         *
         * @returns
         *   Buffer native handle.
         */
        std::any native_handle() const;

    private:

        /** Handle to metal buffer. */
        std::any buffer_;
};

}


