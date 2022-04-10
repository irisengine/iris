////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <memory>
#include <tuple>
#include <unordered_map>

#include "core/utils.h"
#include "graphics/render_graph/render_graph.h"
#include "log/log.h"

namespace iris
{

/**
 * Simple caching class for material objects. Allows for a custom key (KeyParts) allowing the user to choose what
 * constitutes a unique material.
 */
template <class Material, class... KeyParts>
class MaterialCache
{
  public:
    /**
     * Clear the cache.
     */
    void clear()
    {
        materials_.clear();
    }

    /**
     * Try and emplace a new material into the cache with the supplies key parts. Does nothing if an entry already
     * exists.
     *
     * @param key_parts
     *   Data which uniquely identifies the material.
     *
     * @param args
     *   Arguments to construct the material with.
     *
     * @returns
     *   The newly created material if one didn't exist for key_parts, else the material already in the cache.
     */
    template <class... Args>
    Material *try_emplace(KeyParts... key_parts, Args &&...args)
    {
        // construct a key from the supplied parts
        Key k{std::forward<KeyParts>(key_parts)...};

        // check if key exists (so we don't create the material unless we know we're going to insert it)
        if (!materials_.contains(k))
        {
            materials_.try_emplace(k, std::make_unique<Material>(std::forward<Args>(args)...));
        }

        return materials_[k].get();
    }

  private:
    /**
     * Custom key class.
     */
    struct Key
    {
        /**
         * Construct a new key from the supplied parts.
         *
         * @param key_parts
         *   Data which uniquely identifies a material.
         */
        Key(KeyParts... key_parts)
            : parts(std::forward<KeyParts>(key_parts)...)
        {
        }

        /**
         * For the cache to work we need keys to compare equal only if their hashes are the same.
         *
         * @param other
         *   Key to compare with.
         *
         * @returns
         *   True if both keys hash the same, otherwise false.
         */
        bool operator==(const Key &other) const
        {
            const Hash hash{};
            return hash(*this) == hash(other);
        }

        /** Key data. */
        std::tuple<KeyParts...> parts;
    };

    /**
     * Custom hash function.
     */
    struct Hash
    {
        std::size_t operator()(const Key &key) const
        {
            return std::apply(combine_hash<KeyParts...>, key.parts);
        }
    };

    /** Material cache. */
    std::unordered_map<Key, std::unique_ptr<Material>, Hash> materials_;
};

}
