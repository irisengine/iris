#pragma once

#include <any>
#include <cstdint>
#include <memory>

#include "buffer_type.h"
#include "core/data_buffer.h"
#include "core/exception.h"
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
     * Construct a new Buffer from a collection. This will be cast to a byte
     * array and copied to a hardware buffer.
     *
     * @param data
     *   Data to store in buffer.
     *
     * @param type
     *   Semantics of buffer.
     */
    template <class T>
    Buffer(const std::vector<T> &data, const BufferType type)
        : Buffer(
              DataBuffer(
                  reinterpret_cast<const std::byte *>(data.data()),
                  reinterpret_cast<const std::byte *>(data.data()) +
                      sizeof(T) * data.size()),
              type,
              data.size())
    {
    }

    /**
     * Construct a new Buffer from raw bytes.
     *
     * @param data
     *   Data to store in buffer.
     *
     * @param type
     *   Semantics of buffer.
     *
     * @param element_count
     *   The number of elements represented by the raw bytes.
     */
    Buffer(
        const DataBuffer &data,
        const BufferType type,
        std::size_t element_count);

    /** Declared in mm/cpp file as implementation is an incomplete type. */
    ~Buffer();
    Buffer(Buffer &&other);
    Buffer &operator=(Buffer &&other);

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

    /**
     * Return number of elements in the buffer.
     *
     * @param
     *   Number of elements.
     */
    std::size_t element_count() const;

    /**
     * Get a reference to the original data (as raw bytes).
     *
     * @returns
     *   Original data.
     */
    const DataBuffer &data() const;

    /**
     * Get a copy of the original data as collection of a supplied type.
     *
     * Note it is undefined to call this method with any type other than that
     * used by the templated constructor.
     *
     * @returns
     *   Original data as collection of elements.
     */
    template <class T>
    std::vector<T> elements() const
    {
        if (data_.size() != element_count_ * sizeof(T))
        {
            throw Exception("incorrect type");
        }

        std::vector<T> elements(element_count_);

        std::memcpy(elements.data(), data_.data(), data_.size());

        return elements;
    }

  private:
    /** Graphics API implementation. */
    struct implementation;
    std::unique_ptr<implementation> impl_;

    /** Buffer type. */
    BufferType type_;

    /** Number of elements in data_ */
    std::size_t element_count_;

    /** Copy of data in hardware buffer. */
    DataBuffer data_;
};

}
