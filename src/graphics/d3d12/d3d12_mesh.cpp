////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_mesh.h"

#include <memory>
#include <tuple>
#include <vector>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/exception.h"
#include "graphics/vertex_attributes.h"

namespace
{

/**
 * Helper function to map engine attribute types to D3D12 types.
 *
 * @param type
 *   Engine type
 *
 * @returns
 *   D3D12 type.
 */
DXGI_FORMAT to_directx_format(iris::VertexAttributeType type)
{
    auto format = DXGI_FORMAT_UNKNOWN;

    switch (type)
    {
        case iris::VertexAttributeType::FLOAT_3: format = DXGI_FORMAT_R32G32B32_FLOAT; break;
        case iris::VertexAttributeType::FLOAT_4: format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
        case iris::VertexAttributeType::UINT32_1: format = DXGI_FORMAT_R32_UINT; break;
        case iris::VertexAttributeType::UINT32_4: format = DXGI_FORMAT_R32G32B32A32_UINT; break;
        default: throw iris::Exception("unknown vertex attribute type");
    }

    return format;
}
}

namespace iris
{

D3D12Mesh::D3D12Mesh(
    const std::vector<VertexData> &vertices,
    const std::vector<std::uint32_t> &indices,
    const VertexAttributes &attributes)
    : vertex_buffer_(vertices)
    , index_buffer_(indices)
    , input_descriptors_()
{
    // build a D3D12 descriptors from supplied attributes
    auto index = 0u;
    for (const auto &[type, _1, _2, offset] : attributes)
    {
        // the engine doesn't care too much about vertex semantics, so we just
        // call everything a TEXCOORD
        input_descriptors_.push_back(
            {"TEXCOORD",
             index,
             to_directx_format(type),
             0u,
             static_cast<UINT>(offset),
             D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
             0u});

        ++index;
    }
}

void D3D12Mesh::update_vertex_data(const std::vector<VertexData> &data)
{
    vertex_buffer_.write(data);
}

void D3D12Mesh::update_index_data(const std::vector<std::uint32_t> &data)
{
    index_buffer_.write(data);
}

const D3D12Buffer &D3D12Mesh::vertex_buffer() const
{
    return vertex_buffer_;
}

const D3D12Buffer &D3D12Mesh::index_buffer() const
{
    return index_buffer_;
}

std::vector<D3D12_INPUT_ELEMENT_DESC> D3D12Mesh::input_descriptors() const
{
    return input_descriptors_;
}

}
