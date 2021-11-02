////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <cstring>
#include <iterator>
#include <memory>
#include <type_traits>

#include "core/data_buffer.h"
#include "core/exception.h"
#include "core/quaternion.h"
#include "core/vector3.h"

namespace iris
{

/**
 * Class for deserialising types stored in a DataBuffer. This is the inverse
 * operation to DataBufferSerialiser.
 */
class DataBufferDeserialiser
{
  public:
    /**
     * Construct a new DataBufferDeserialiser.
     *
     * @param buffer
     *   DataBuffer of serialised data.
     */
    explicit DataBufferDeserialiser(DataBuffer buffer)
        : buffer_(std::move(buffer))
        , cursor_(std::cbegin(buffer_))
    {
    }

    /**
     * Pop integral type.
     *
     * @returns
     *   Next element in buffer as supplied type.
     */
    template <class T, std::enable_if_t<std::is_arithmetic_v<T>> * = nullptr>
    T pop()
    {
        const auto size = static_cast<std::ptrdiff_t>(sizeof(T));
        if (size > std::distance(cursor_, std::cend(buffer_)))
        {
            throw Exception("not enough data left");
        }

        T value{};
        std::memcpy(&value, std::addressof(*cursor_), size);

        cursor_ += size;

        return value;
    }

    /**
     * Pop enum.
     *
     * @returns
     *   Next element in buffer as supplied type.
     */
    template <class T, std::enable_if_t<std::is_enum_v<T>> * = nullptr>
    T pop()
    {
        using type = std::underlying_type_t<T>;

        const auto value = pop<type>();

        return static_cast<T>(value);
    }

    /**
     * Pop Vector3.
     *
     * @returns
     *   Next element in buffer as supplied type.
     */
    template <class T, std::enable_if_t<std::is_same_v<T, Vector3>> * = nullptr>
    T pop()
    {
        return T{pop<float>(), pop<float>(), pop<float>()};
    }

    /**
     * Pop Quaternion.
     *
     * @returns
     *   Next element in buffer as supplied type.
     */
    template <class T, std::enable_if_t<std::is_same_v<T, Quaternion>> * = nullptr>
    T pop()
    {
        return T{pop<float>(), pop<float>(), pop<float>(), pop<float>()};
    }

    /**
     * Pop DataBuffer.
     *
     * @returns
     *   Next element in buffer as supplied type.
     */
    template <class T, std::enable_if_t<std::is_same_v<T, DataBuffer>> * = nullptr>
    T pop()
    {
        const auto size = pop<std::uint32_t>();

        DataBuffer value(cursor_, cursor_ + size);
        cursor_ += size;

        return value;
    }

    /**
     * Pop tuple of supplied types.
     *
     * @returns
     *   std::tuple of requested types from buffer.
     */
    template <class... Types>
    std::tuple<Types...> pop_tuple()
    {
        std::tuple<Types...> values;

        pop_tuple_impl<0u, std::tuple<Types...>, Types...>(values);

        return values;
    }

  private:
    /**
     * Helper recursive template method. Sets the next requested tuple
     * element.
     *
     * @param values
     *   Tuple of elements to append to.
     */
    template <std::size_t Index, class T, class Head, class... Tail>
    void pop_tuple_impl(T &values)
    {
        // set next tuple element
        std::get<Index>(values) = pop<Head>();

        // recurse to set next element
        pop_tuple_impl<Index + 1u, T, Tail...>(values);
    }

    /**
     * Base method for template recursion.
     */
    template <std::size_t Index, class T>
    void pop_tuple_impl(T &)
    {
    }

    /** Buffer of serialised data. */
    DataBuffer buffer_;

    /** Iterator into buffer, where next element will be popped from. */
    DataBuffer::const_iterator cursor_;
};

}
