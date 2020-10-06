#include "graphics/vertex_attributes.h"

#include <cstddef>
#include <iterator>
#include <tuple>
#include <vector>

#include "core/exception.h"
#include "core/vector3.h"

namespace
{
/**
 * Helper function to convert a VertexAttributeType to a tuple of:
 * <number of components, size of singe component>
 *
 * @param type
 *   Type to convert.
 *
 * @returns
 *   Tuple of <number of components, size of single component>.
 */
std::tuple<std::size_t, std::size_t> type_info(iris::VertexAttributeType type)
{
    std::tuple<std::size_t, std::size_t> info(0u, 0u);

    switch (type)
    {
        case iris::VertexAttributeType::FLOAT_3:
            info = {3u, sizeof(float)};
            break;
        case iris::VertexAttributeType::FLOAT_4:
            info = {4u, sizeof(float)};
            break;
        case iris::VertexAttributeType::UINT32_1:
            info = {1u, sizeof(std::uint32_t)};
            break;
        case iris::VertexAttributeType::UINT32_4:
            info = {4u, sizeof(std::uint32_t)};
            break;
        default:
            throw iris::Exception("unknown vertex attribute type");
    }

    return info;
}
}

namespace iris
{

VertexAttributes::VertexAttributes(
    const std::vector<VertexAttributeType> &types)
    : attributes_()
    , size_(0u)
{
    std::size_t offset = 0u;

    for (const auto type : types)
    {
        const auto [components, size] = type_info(type);
        attributes_.emplace_back(
            VertexAttribute{type, components, size, offset});
        offset += components * size;
    }

    size_ = offset;
}

std::size_t VertexAttributes::size() const
{
    return size_;
}

VertexAttributes::const_iterator VertexAttributes::begin() const
{
    return cbegin();
}

VertexAttributes::const_iterator VertexAttributes::end() const
{
    return cend();
}

VertexAttributes::const_iterator VertexAttributes::cbegin() const
{
    return std::cbegin(attributes_);
}

VertexAttributes::const_iterator VertexAttributes::cend() const
{
    return std::cend(attributes_);
}

}
