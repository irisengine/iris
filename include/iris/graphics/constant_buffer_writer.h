#pragma once

#include <array>
#include <cstddef>

namespace iris
{

/**
 * This class provides a convenient way of writing to some (graphics api
 * specific) constant data buffer. It is designed such that subsequent calls to
 * write() will automatically advance, so the buffer can be filled by repeated
 * calls.
 */
template <class T>
class ConstantBufferWriter
{
  public:
    /**
     * Construct a new ConstantBufferWriter.
     *
     * @param buffer
     *   Buffer to write data to.
     */
    ConstantBufferWriter(T &buffer)
        : buffer_(buffer)
        , offset_(0u)
    {
    }

    /**
     * Write an object to the buffer at the current position.
     *
     * @param object
     *   Object to write.
     */
    template <class S>
    void write(const S &object)
    {
        buffer_.write(object, offset_);
        offset_ += sizeof(S);
    }

    /**
     * Write a collection of objects to the buffer at the current position.
     *
     * @param objects
     *   Objects to write.
     */
    template <class S>
    void write(const std::vector<S> &objects)
    {
        buffer_.write(objects.data(), sizeof(S) * objects.size(), offset_);
        offset_ += sizeof(S) * objects.size();
    }

    /**
     * Write an array of objects to the buffer at the current position.
     *
     * @param objects
     *   Objects to write.
     */
    template <class S, std::size_t N>
    void write(const std::array<S, N> &objects)
    {
        buffer_.write(objects.data(), sizeof(S) * N, offset_);
        offset_ += sizeof(S) * objects.size();
    }

    /**
     * Advance the internal offset into the buffer.
     *
     * @param offset
     *   Amount (in bytes) to increment internal offset.
     */
    void advance(std::size_t offset)
    {
        offset_ += offset;
    }

  private:
    /** Buffer to write to, */
    T &buffer_;

    /** Offset into buffer to write to. */
    std::size_t offset_;
};

}
