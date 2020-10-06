#pragma once

#include <cstdint>
#include <vector>

#include "core/vector3.h"

namespace iris
{

/**
 * Enumeration of attribute types.
 */
enum class VertexAttributeType
{
    FLOAT_3,
    FLOAT_4,
    UINT32_1,
    UINT32_4
};

/**
 * Struct encapsulating data needed to describe a vertex attribute.
 */
struct VertexAttribute
{
    /** Attribute type. */
    VertexAttributeType type;

    /** Number of components in attribute, typically 1, 2, 3 or 4. */
    std::size_t components;

    /** Size of attribute (size(type) * components). */
    std::size_t size;

    /** Number of bytes from start of attribute collection. */
    std::size_t offset;
};

/**
 * Class encapsulating a series of vertex attributes. These describe how to
 * interpret a vertex buffer, which is just a series of bytes.
 */
class VertexAttributes
{
  public:
    //  useful aliases
    using attributes = std::vector<VertexAttribute>;
    using const_iterator = attributes::const_iterator;

    /**
     * Construct a new VertexAttributes.
     *
     * @param attributes
     *   Collection of attributes.
     */
    VertexAttributes(const std::vector<VertexAttributeType> &attributes);

    /**
     * Get size of all attributes. This will be the same as the size of a buffer
     * containing all vertex data.
     *
     * @returns
     *   Size of attributes.
     */
    std::size_t size() const;

    /**
     * Get iterator to start if attributes.
     *
     * Note that this class is immutable so this returns a const_iterator.
     *
     * @returns
     *   Iterator to start of attributes.
     */
    const_iterator begin() const;

    /**
     * Get iterator to end if attributes.
     *
     * Note that this class is immutable so this returns a const_iterator.
     *
     * @returns
     *   Iterator to end of attributes.
     */
    const_iterator end() const;

    /**
     * Get iterator to start if attributes.
     *
     * @returns
     *   Iterator to start of attributes.
     */
    const_iterator cbegin() const;

    /**
     * Get iterator to end if attributes.
     *
     * @returns
     *   Iterator to end of attributes.
     */
    const_iterator cend() const;

  private:
    /** Collection of attributes. */
    std::vector<VertexAttribute> attributes_;

    /** Size of attributes. */
    std::size_t size_;
};

}
