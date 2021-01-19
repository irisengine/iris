#include "graphics/mesh.h"

#import <Metal/Metal.h>

#include "core/exception.h"

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
        case iris::VertexAttributeType::FLOAT_3:
            format = MTLVertexFormatFloat3;
            break;
        case iris::VertexAttributeType::FLOAT_4:
            format = MTLVertexFormatFloat4;
            break;
        case iris::VertexAttributeType::UINT32_1:
            format = MTLVertexFormatUInt;
            break;
        case iris::VertexAttributeType::UINT32_4:
            format = MTLVertexFormatUInt4;
            break;
        default:
            throw iris::Exception("unknown vertex attribute type");
    }

    return format;
}

}

namespace iris
{

struct Mesh::implementation
{
    MTLVertexDescriptor *descriptor;
};

Mesh::Mesh( 
    Buffer vertex_buffer,
    Buffer index_buffer,
    const VertexAttributes &attributes)
    : vertex_buffer_(std::move(vertex_buffer))
    , index_buffer_(std::move(index_buffer))
    , impl_(std::make_unique<implementation>())
{
    impl_->descriptor = [[MTLVertexDescriptor alloc] init];

    auto index = 0u;

    // convert engine attribute information to metal
    for(const auto &[type, components, size, offset] : attributes)
    {
        impl_->descriptor.attributes[index].format = to_metal_format(type);
        impl_->descriptor.attributes[index].offset = offset;
        impl_->descriptor.attributes[index].bufferIndex = index;

        ++index;
    }

    impl_->descriptor.layouts[0u].stride = attributes.size();
}

Mesh::~Mesh() = default;
Mesh::Mesh(Mesh&&) = default;
Mesh& Mesh::operator=(Mesh&&) = default;

const Buffer &Mesh::vertex_buffer() const
{
    return vertex_buffer_;
}

const Buffer &Mesh::index_buffer() const
{
    return index_buffer_;
}

std::any Mesh::native_handle() const
{
    return { impl_->descriptor };
}

}
