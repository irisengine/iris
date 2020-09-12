#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <type_traits>

#include "core/data_buffer.h"
#include "core/vector3.h"
#include "core/quaternion.h"

namespace iris
{

/**
 * Class for serialising types to a DataBuffer.
 * 
 * Note this assumes little-endian.
 */
class DataBufferSerialiser
{
    public:

        /**
         * Get the serialised data.
         * 
         * @returns
         *   DataBuffer of serialised data.
         */
        DataBuffer data() const
        {
            return buffer_;
        }

        /**
         * Serialise an integral type.
         * 
         * @param value
         *   Value to serialise.
         */
        template<class T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
        void push(T value)
        {
            const auto size = sizeof(T);
            buffer_.resize(buffer_.size() + size);
            std::memcpy(
                std::addressof(*(std::end(buffer_) - size)),
                &value,
                size);
        }

        /**
         * Serialise an enum.
         * 
         * @param value
         *   Value to serialise.
         */
        template<class T, std::enable_if_t<std::is_enum_v<T>>* = nullptr>
        void push(T value)
        {
            using type = std::underlying_type_t<T>;

            push<type>(static_cast<type>(value));
        }

        /**
         * Serialise a real.
         * 
         * @param value
         *   Value to serialise.
         */
        void push(real value)
        {
            push(value.value);
        }

        /**
         * Serialise a Vector3.
         * 
         * @param value
         *   Value to serialise.
         */
        void push(const Vector3 &value)
        {
            push(value.x);
            push(value.y);
            push(value.z);
        }

        /**
         * Serialise a Quaternion.
         * 
         * @param value
         *   Value to serialise.
         */
        void push(const Quaternion &value)
        {
            push(value.x);
            push(value.y);
            push(value.z);
            push(value.w);
        }

        /**
         * Serialise a DataBuffer.
         * 
         * @param value
         *   Value to serialise.
         */
        void push(const DataBuffer &value)
        {
            const auto size = value.size();
            push(static_cast<std::uint32_t>(size));
            buffer_.resize(buffer_.size() + size);
            std::memcpy(
                std::addressof(*(std::end(buffer_) - size)),
                value.data(),
                size);
        }

    private:

        /** Serialised data. */
        DataBuffer buffer_;

};

}
