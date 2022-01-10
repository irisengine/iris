////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/metal/metal_mesh.h"

#include <cstdint>
#include <vector>

#import <Metal/Metal.h>

#include "core/exception.h"
#include "graphics/mesh.h"
#include "graphics/metal/metal_buffer.h"
#include "graphics/vertex_data.h"

namespace
{

/**
 * Helper function to convert engine attribute type to metal attribute type.
 *
 * @param type
 *   Engine type.
 *
 * @returns
 *    Metal type.
 */
MTLVertexFormat to_metal_format(iris::VertexAttributeType type)
{
    auto format = MTLVertexFormatInvalid;

    switch (type)
    {
        case iris::VertexAttributeType::FLOAT_3: format = MTLVertexFormatFloat3; break;
        case iris::VertexAttributeType::FLOAT_4: format = MTLVertexFormatFloat4; break;
        case iris::VertexAttributeType::UINT32_1: format = MTLVertexFormatUInt; break;
        case iris::VertexAttributeType::UINT32_4: format = MTLVertexFormatUInt4; break;
        default: throw iris::Exception("unknown vertex attribute type");
    }

    return format;
}

}

namespace iris
{

MetalMesh::MetalMesh(
    const std::vector<VertexData> &vertices,
    const std::vector<std::uint32_t> &indices,
    const VertexAttributes &attributes)
    : Mesh(vertices, indices)
    , vertex_buffer_(vertices_)
    , index_buffer_(indices_)
    , descriptors_(nullptr)
{
    descriptors_ = [[MTLVertexDescriptor alloc] init];

    auto index = 0u;

    // convert engine attribute information to metal
    for (const auto &[type, components, size, offset] : attributes)
    {
        descriptors_.attributes[index].format = to_metal_format(type);
        descriptors_.attributes[index].offset = offset;
        descriptors_.attributes[index].bufferIndex = 0u;

        ++index;
    }

    descriptors_.layouts[0u].stride = attributes.size();
}

void MetalMesh::update_vertex_data(const std::vector<VertexData> &data)
{
    vertex_buffer_.write(data);
}

void MetalMesh::update_index_data(const std::vector<std::uint32_t> &data)
{
    index_buffer_.write(data);
}

const MetalBuffer &MetalMesh::vertex_buffer() const
{
    return vertex_buffer_;
}

const MetalBuffer &MetalMesh::index_buffer() const
{
    return index_buffer_;
}

MTLVertexDescriptor *MetalMesh::descriptors() const
{
    return descriptors_;
}

}
