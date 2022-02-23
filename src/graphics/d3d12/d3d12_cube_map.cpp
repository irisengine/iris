////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_cube_map.h"

#include <cstdint>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/data_buffer.h"
#include "core/error_handling.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/d3d12/d3d12_descriptor_manager.h"

namespace iris
{

D3D12CubeMap::D3D12CubeMap(
    const DataBuffer &right_data,
    const DataBuffer &left_data,
    const DataBuffer &top_data,
    const DataBuffer &bottom_data,
    const DataBuffer &back_data,
    const DataBuffer &front_data,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t index)
    : CubeMap(index)
    , resource_()
    , upload_()
    , resource_view_()
    , type_()
{
    auto *device = D3D12Context::device();
    const auto default_heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_RESOURCE_DESC texture_description{};
    texture_description.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    texture_description.Width = width;
    texture_description.Height = height;
    texture_description.Flags = D3D12_RESOURCE_FLAG_NONE;
    texture_description.DepthOrArraySize = 6;
    texture_description.MipLevels = 1;
    texture_description.SampleDesc.Count = 1;
    texture_description.SampleDesc.Quality = 0;
    texture_description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texture_description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texture_description.Alignment = 0;

    // create a resource where image data will be coped to
    expect(
        device->CreateCommittedResource(
            &default_heap,
            D3D12_HEAP_FLAG_NONE,
            &texture_description,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&resource_)) == S_OK,
        "could not create resource");

    const UINT64 capacity = GetRequiredIntermediateSize(resource_.Get(), 0, 6);

    const auto upload_heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    const auto heap_description = CD3DX12_RESOURCE_DESC::Buffer(capacity);

    // create resource for initial upload of texture data
    expect(
        device->CreateCommittedResource(
            &upload_heap,
            D3D12_HEAP_FLAG_NONE,
            &heap_description,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&upload_)) == S_OK,
        "could not create resource");

    type_ = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    resource_view_ = D3D12DescriptorManager::cpu_allocator(type_).allocate_static();

    D3D12_SHADER_RESOURCE_VIEW_DESC shader_view_description = {0};
    shader_view_description.Format = texture_description.Format;
    shader_view_description.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    shader_view_description.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    shader_view_description.TextureCube.MipLevels = 1;
    shader_view_description.TextureCube.MostDetailedMip = 0;
    shader_view_description.TextureCube.ResourceMinLODClamp = 0.0f;

    device->CreateShaderResourceView(resource_.Get(), &shader_view_description, resource_view_.cpu_handle());

    // map the upload buffer so we can write to it
    void *mapped_buffer = nullptr;
    expect(upload_->Map(0u, NULL, reinterpret_cast<void **>(&mapped_buffer)) == S_OK, "failed to map constant buffer");

    const std::byte *data_ptrs[] = {
        right_data.data(), left_data.data(), top_data.data(), bottom_data.data(), back_data.data(), front_data.data()};

    UINT heights[] = {height, height, height, height, height, height};
    UINT64 row_sizes[] = {width * 4u, width * 4u, width * 4u, width * 4u, width * 4u, width * 4u};

    // create footprint for image data layout
    std::uint64_t memory_size = 0u;
    device->GetCopyableFootprints(
        &texture_description,
        0,
        static_cast<UINT>(footprints_.size()),
        0,
        footprints_.data(),
        heights,
        row_sizes,
        &memory_size);

    for (auto i = 0u; i < 6u; ++i)
    {
        auto *dst_cursor = reinterpret_cast<std::byte *>(mapped_buffer) + footprints_[i].Offset;
        auto *src_cursor = data_ptrs[i];

        // copy texture data with respect to footprint
        for (auto j = 0u; j < heights[i]; ++j)
        {
            std::memcpy(dst_cursor, src_cursor, static_cast<std::size_t>(row_sizes[i]));
            dst_cursor += footprints_[i].Footprint.RowPitch;
            src_cursor += row_sizes[i];
        }
    }
}

ID3D12Resource *D3D12CubeMap::resource() const
{
    return resource_.Get();
}

ID3D12Resource *D3D12CubeMap::upload() const
{
    return upload_.Get();
}

D3D12DescriptorHandle D3D12CubeMap::handle() const
{
    return resource_view_;
}

std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT, 6u> D3D12CubeMap::footprints() const
{
    return footprints_;
}

}
