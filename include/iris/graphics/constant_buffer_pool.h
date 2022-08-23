////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>

#include "core/error_handling.h"

namespace iris
{

template <class T, std::size_t N = 1000, class Allocator = std::allocator<T>>
class ConstantBufferPool
{
  public:
    ConstantBufferPool()
        : chunk_alloc_()
        , chunks_(nullptr)
        , free_list_(nullptr)
    {
        chunks_ = chunk_alloc_.allocate(N);

        auto *cursor = chunks_;
        for (auto i = 0u; i < N; i++)
        {
            ::new (std::addressof(cursor->buffer)) T(1024u * 8u);
            ++cursor;
        }

        free_list_ = chunks_;
    }

    T *next()
    {
        ensure(free_list_ != nullptr, "pool has been drained");

        auto *object = std::addressof(free_list_->buffer);

        if (free_list_->next == nullptr)
        {
            ++free_list_;
        }
        else
        {
            free_list_ = free_list_->next;
        }

        return object;
    }

    void release(T *object)
    {
        auto *chunk = reinterpret_cast<Chunk *>(object) - 1u;

        chunk->next = free_list_;
        free_list_ = chunk;
    }

  private:
    struct Chunk
    {
        Chunk(std::size_t size)
            : next(nullptr)
            , buffer(size)
        {
        }

        Chunk *next;
        T buffer;
    };

    typename std::allocator_traits<Allocator>::template rebind_alloc<Chunk> chunk_alloc_;

    Chunk *chunks_;

    Chunk *free_list_;
};

}
