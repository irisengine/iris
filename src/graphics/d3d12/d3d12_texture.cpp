#include "graphics/d3d12/d3d12_texture.h"

#include <sstream>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/colour.h"
#include "core/exception.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/d3d12/d3d12_descriptor_manager.h"
#include "graphics/pixel_format.h"

namespace iris
{

D3D12Texture::D3D12Texture(
    const DataBuffer &data,
    std::uint32_t width,
    std::uint32_t height,
    PixelFormat pixel_format,
    bool is_render_target,
    std::uint32_t samples)
    : Texture(data, width, height, pixel_format)
    , resource_()
    , upload_()
    , resource_view_()
    , depth_resource_view_()
    , footprint_()
    , type_()
{
    if (samples == 0u)
    {
        samples = 1u;
    }

    // create description of texture resource
    D3D12_RESOURCE_DESC texture_description{};
    texture_description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture_description.Width = width_;
    texture_description.Height = height_;
    texture_description.Flags = is_render_target
                                    ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
                                    : D3D12_RESOURCE_FLAG_NONE;
    texture_description.DepthOrArraySize = 1;
    texture_description.MipLevels = 1;
    texture_description.SampleDesc.Count = samples;
    texture_description.SampleDesc.Quality = samples > 1u ? 1u : 0u;
    texture_description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texture_description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texture_description.Alignment = 0;

    auto *device = D3D12Context::device();

    const auto default_heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    const Colour clear_colour{0.4f, 0.6f, 0.9f, 1.0f};
    D3D12_CLEAR_VALUE clear_value = {0};
    clear_value.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    std::memcpy(&clear_value.Color, &clear_colour, sizeof(clear_colour));

    // create a resource where image data will be coped to
    if (device->CreateCommittedResource(
            &default_heap,
            D3D12_HEAP_FLAG_NONE,
            &texture_description,
            is_render_target ? D3D12_RESOURCE_STATE_RENDER_TARGET
                             : D3D12_RESOURCE_STATE_COPY_DEST,
            is_render_target ? &clear_value : nullptr,
            IID_PPV_ARGS(&resource_)) != S_OK)
    {
        throw Exception("could not create resource");
    }

    const UINT64 capacity = GetRequiredIntermediateSize(resource_.Get(), 0, 1);

    const auto upload_heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    const auto heap_description = CD3DX12_RESOURCE_DESC::Buffer(capacity);

    // create resource for initial upload of texture data
    if (device->CreateCommittedResource(
            &upload_heap,
            D3D12_HEAP_FLAG_NONE,
            &heap_description,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&upload_)) != S_OK)
    {
        throw Exception("could not create resource");
    }

    type_ = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    resource_view_ =
        D3D12DescriptorManager::cpu_allocator(type_).allocate_static();

    device->CreateShaderResourceView(
        resource_.Get(), NULL, resource_view_.cpu_handle());

    // map the upload buffer so we can write to it
    void *mapped_buffer = nullptr;
    if (upload_->Map(0u, NULL, reinterpret_cast<void **>(&mapped_buffer)) !=
        S_OK)
    {
        throw Exception("failed to map constant buffer");
    }

    UINT heights[] = {height_};
    UINT64 row_size[] = {width_ * 4u};

    // create footprint for image data layout
    std::uint64_t memory_size = 0u;
    device->GetCopyableFootprints(
        &texture_description,
        0,
        1,
        0,
        &footprint_,
        heights,
        row_size,
        &memory_size);

    auto *dst_cursor = reinterpret_cast<std::byte *>(mapped_buffer);
    auto *src_cursor = data_.data();

    // copy texture data with respect to footprint
    for (auto i = 0u; i < height_; ++i)
    {
        std::memcpy(dst_cursor, src_cursor, row_size[0]);
        dst_cursor += footprint_.Footprint.RowPitch;
        src_cursor += row_size[0];
    }

    static int c = 0;
    std::wstringstream strm{};
    strm << L"colour_";
    if (is_render_target)
    {
        strm << L"rt_";
    }
    strm << c++;
    name_ = strm.str();
    resource_->SetName(name_.c_str());
}

D3D12Texture::D3D12Texture(
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t samples)
    : Texture({}, width, height, PixelFormat::DEPTH)
{
    if (samples == 0u)
    {
        samples = 1u;
    }

    // create description of depth buffer resource
    D3D12_RESOURCE_DESC texture_description{};
    texture_description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texture_description.Alignment = 0;
    texture_description.Width = width_;
    texture_description.Height = height_;
    texture_description.DepthOrArraySize = 1;
    texture_description.MipLevels = 1;
    texture_description.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texture_description.SampleDesc.Count = samples;
    texture_description.SampleDesc.Quality = samples > 1u ? 1u : 0u;
    texture_description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texture_description.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clear_value = {};
    clear_value.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    clear_value.DepthStencil.Depth = 1.0f;
    clear_value.DepthStencil.Stencil = 0u;

    auto *device = D3D12Context::device();

    const auto default_heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    // we don't have any data to upload so we can create the resource directly
    // in the default heap
    if (device->CreateCommittedResource(
            &default_heap,
            D3D12_HEAP_FLAG_NONE,
            &texture_description,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            &clear_value,
            IID_PPV_ARGS(&resource_)) != S_OK)
    {
        throw Exception("could not create resource");
    }

    type_ = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

    depth_resource_view_ =
        D3D12DescriptorManager::cpu_allocator(type_).allocate_static();

    D3D12_DEPTH_STENCIL_VIEW_DESC depth_view_description = {0};
    depth_view_description.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_view_description.ViewDimension = samples > 1u
                                               ? D3D12_DSV_DIMENSION_TEXTURE2DMS
                                               : D3D12_DSV_DIMENSION_TEXTURE2D;
    depth_view_description.Flags = D3D12_DSV_FLAG_NONE;
    depth_view_description.Texture2D.MipSlice = 0;

    // create the depth/stencil view into the texture
    device->CreateDepthStencilView(
        resource_.Get(),
        &depth_view_description,
        depth_resource_view_.cpu_handle());

    resource_view_ = D3D12DescriptorManager::cpu_allocator(
                         D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
                         .allocate_static();

    D3D12_SHADER_RESOURCE_VIEW_DESC shader_view_description = {0};
    shader_view_description.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    shader_view_description.Shader4ComponentMapping =
        D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    shader_view_description.ViewDimension =
        samples > 1u ? D3D12_SRV_DIMENSION_TEXTURE2DMS
                     : D3D12_SRV_DIMENSION_TEXTURE2D;
    shader_view_description.Texture2D.MipLevels = 1;
    shader_view_description.Texture2D.MostDetailedMip = 0;

    device->CreateShaderResourceView(
        resource_.Get(), &shader_view_description, resource_view_.cpu_handle());

    static int c = 0;
    std::wstringstream strm{};
    strm << L"depth_rt_" << c++;
    name_ = strm.str();
    resource_->SetName(name_.c_str());
}

ID3D12Resource *D3D12Texture::resource() const
{
    return resource_.Get();
}

ID3D12Resource *D3D12Texture::upload() const
{
    return upload_.Get();
}

D3D12_PLACED_SUBRESOURCE_FOOTPRINT D3D12Texture::footprint() const
{
    return footprint_;
}

D3D12DescriptorHandle D3D12Texture::handle() const
{
    return resource_view_;
}

D3D12DescriptorHandle D3D12Texture::depth_handle() const
{
    return depth_resource_view_;
}

D3D12_DESCRIPTOR_HEAP_TYPE D3D12Texture::type() const
{
    return type_;
}

}
