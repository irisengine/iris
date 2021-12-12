////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/d3d12/d3d12_texture.h"

#include <sstream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include "core/colour.h"
#include "core/error_handling.h"
#include "graphics/d3d12/d3d12_context.h"
#include "graphics/d3d12/d3d12_descriptor_handle.h"
#include "graphics/d3d12/d3d12_descriptor_manager.h"
#include "graphics/texture_usage.h"

namespace
{

/**
 * Helper function to set the name of the ID3D12Resource.
 *
 * The prefix will have a unique integer appended, hence why this is templated,
 * so each enum type gets its own range of numbers.
 *
 * @param prefix
 *   Prefix for name.
 *
 * @param resource
 *   The resource to set the name of.
 */
template <iris::TextureUsage T>
void set_name(const std::wstring &prefix, ID3D12Resource *resource)
{
    static int counter = 0;

    std::wstringstream strm{};
    strm << prefix << L"_" << counter++;
    const auto name = strm.str();

    resource->SetName(name.c_str());
}

/**
 * Helper function to create a d3d12 resource description suitable for the
 * texture usage.
 *
 * @param width
 *   Width of texture.
 *
 * @param height
 *   Height of texture.
 *
 * @param resource
 *   Handle to store created resource in.
 *
 * @returns
 *   D3D12_RESOURCE_DESC for texture.
 */
D3D12_RESOURCE_DESC image_texture_descriptor(
    std::uint32_t width,
    std::uint32_t height,
    Microsoft::WRL::ComPtr<ID3D12Resource> &resource)
{
    auto *device = iris::D3D12Context::device();
    const auto default_heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_RESOURCE_DESC texture_description{};
    texture_description.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    texture_description.Width = width;
    texture_description.Height = height;
    texture_description.Flags = D3D12_RESOURCE_FLAG_NONE;
    texture_description.DepthOrArraySize = 1;
    texture_description.MipLevels = 1;
    texture_description.SampleDesc.Count = 1;
    texture_description.SampleDesc.Quality = 0;
    texture_description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texture_description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texture_description.Alignment = 0;

    // create a resource where image data will be coped to
    iris::expect(
        device->CreateCommittedResource(
            &default_heap,
            D3D12_HEAP_FLAG_NONE,
            &texture_description,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&resource)) == S_OK,
        "could not create resource");

    set_name<iris::TextureUsage::IMAGE>(L"tex", resource.Get());

    return texture_description;
}

/**
 * Helper function to create a d3d12 resource description suitable for the
 * data usage.
 *
 * @param width
 *   Width of texture.
 *
 * @param height
 *   Height of texture.
 *
 * @param resource
 *   Handle to store created resource in.
 *
 * @returns
 *   D3D12_RESOURCE_DESC for texture.
 */
D3D12_RESOURCE_DESC data_texture_descriptor(
    std::uint32_t width,
    std::uint32_t height,
    Microsoft::WRL::ComPtr<ID3D12Resource> &resource)
{
    auto *device = iris::D3D12Context::device();
    const auto default_heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_RESOURCE_DESC texture_description{};
    texture_description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture_description.Width = width;
    texture_description.Height = height;
    texture_description.Flags = D3D12_RESOURCE_FLAG_NONE;
    texture_description.DepthOrArraySize = 1;
    texture_description.MipLevels = 1;
    texture_description.SampleDesc.Count = 1;
    texture_description.SampleDesc.Quality = 0;
    texture_description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texture_description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texture_description.Alignment = 0;

    // create a resource where image data will be coped to
    iris::expect(
        device->CreateCommittedResource(
            &default_heap,
            D3D12_HEAP_FLAG_NONE,
            &texture_description,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&resource)) == S_OK,
        "could not create resource");

    set_name<iris::TextureUsage::IMAGE>(L"tex", resource.Get());

    return texture_description;
}

/**
 * Helper function to create a d3d12 resource description suitable for the
 * render target usage.
 *
 * @param width
 *   Width of texture.
 *
 * @param height
 *   Height of texture.
 *
 * @param resource
 *   Handle to store created resource in.
 *
 * @returns
 *   D3D12_RESOURCE_DESC for texture.
 */
D3D12_RESOURCE_DESC
render_target_texture_descriptor(
    std::uint32_t width,
    std::uint32_t height,
    Microsoft::WRL::ComPtr<ID3D12Resource> &resource)
{
    auto *device = iris::D3D12Context::device();
    const auto default_heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_RESOURCE_DESC texture_description{};
    texture_description.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    texture_description.Width = width;
    texture_description.Height = height;
    texture_description.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    texture_description.DepthOrArraySize = 1;
    texture_description.MipLevels = 1;
    texture_description.SampleDesc.Count = 1;
    texture_description.SampleDesc.Quality = 0;
    texture_description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texture_description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texture_description.Alignment = 0;

    const iris::Colour clear_colour{0.4f, 0.6f, 0.9f, 1.0f};
    D3D12_CLEAR_VALUE clear_value = {0};
    clear_value.Format = texture_description.Format;
    std::memcpy(&clear_value.Color, &clear_colour, sizeof(clear_colour));

    // create a resource where image data will be coped to
    iris::expect(
        device->CreateCommittedResource(
            &default_heap,
            D3D12_HEAP_FLAG_NONE,
            &texture_description,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            &clear_value,
            IID_PPV_ARGS(&resource)) == S_OK,
        "could not create resource");

    set_name<iris::TextureUsage::IMAGE>(L"rt", resource.Get());

    return texture_description;
}

/**
 * Helper function to create a d3d12 resource description suitable for the
 * depth usage.
 *
 * @param width
 *   Width of texture.
 *
 * @param height
 *   Height of texture.
 *
 * @param resource
 *   Handle to store created resource in.
 *
 * @returns
 *   D3D12_RESOURCE_DESC for texture.
 */
D3D12_RESOURCE_DESC depth_texture_descriptor(
    std::uint32_t width,
    std::uint32_t height,
    Microsoft::WRL::ComPtr<ID3D12Resource> &resource)
{
    auto *device = iris::D3D12Context::device();
    const auto default_heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_RESOURCE_DESC texture_description{};
    texture_description.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texture_description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texture_description.Alignment = 0;
    texture_description.Width = width;
    texture_description.Height = height;
    texture_description.DepthOrArraySize = 1;
    texture_description.MipLevels = 1;
    texture_description.SampleDesc.Count = 1;
    texture_description.SampleDesc.Quality = 0;
    texture_description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texture_description.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clear_value = {};
    clear_value.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    clear_value.DepthStencil.Depth = 1.0f;
    clear_value.DepthStencil.Stencil = 0u;

    iris::expect(
        device->CreateCommittedResource(
            &default_heap,
            D3D12_HEAP_FLAG_NONE,
            &texture_description,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            &clear_value,
            IID_PPV_ARGS(&resource)) == S_OK,
        "could not create resource");

    set_name<iris::TextureUsage::IMAGE>(L"depth", resource.Get());

    return texture_description;
}

}

namespace iris
{

D3D12Texture::D3D12Texture(const DataBuffer &data, std::uint32_t width, std::uint32_t height, TextureUsage usage)
    : Texture(data, width, height, usage)
    , resource_()
    , upload_()
    , resource_view_()
    , depth_resource_view_()
    , footprint_()
    , type_()
{
    auto *device = D3D12Context::device();
    const auto default_heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    const auto &texture_description = [this, usage]() {
        switch (usage)
        {
            case TextureUsage::IMAGE: return image_texture_descriptor(width_, height_, resource_); break;
            case TextureUsage::DATA: return data_texture_descriptor(width_, height_, resource_); break;
            case TextureUsage::RENDER_TARGET:
                return render_target_texture_descriptor(width_, height_, resource_);
                break;
            case TextureUsage::DEPTH: return depth_texture_descriptor(width_, height_, resource_); break;
            default: throw Exception("unknown texture usage");
        }
    }();

    // finish off setting up the resource, note that DEPTH has some some special
    // case handling

    if (usage != TextureUsage::DEPTH)
    {
        const UINT64 capacity = GetRequiredIntermediateSize(resource_.Get(), 0, 1);

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

        device->CreateShaderResourceView(resource_.Get(), NULL, resource_view_.cpu_handle());

        // map the upload buffer so we can write to it
        void *mapped_buffer = nullptr;
        expect(
            upload_->Map(0u, NULL, reinterpret_cast<void **>(&mapped_buffer)) == S_OK, "failed to map constant buffer");

        if (!data.empty())
        {
            UINT heights[] = {height_};
            UINT64 row_size[] = {width_ * 4u};

            // create footprint for image data layout
            std::uint64_t memory_size = 0u;
            device->GetCopyableFootprints(&texture_description, 0, 1, 0, &footprint_, heights, row_size, &memory_size);

            auto *dst_cursor = reinterpret_cast<std::byte *>(mapped_buffer);
            auto *src_cursor = data_.data();

            // copy texture data with respect to footprint
            for (auto i = 0u; i < height_; ++i)
            {
                std::memcpy(dst_cursor, src_cursor, row_size[0]);
                dst_cursor += footprint_.Footprint.RowPitch;
                src_cursor += row_size[0];
            }
        }
    }
    else
    {
        type_ = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

        depth_resource_view_ = D3D12DescriptorManager::cpu_allocator(type_).allocate_static();

        D3D12_DEPTH_STENCIL_VIEW_DESC depth_view_description = {0};
        depth_view_description.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_view_description.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depth_view_description.Flags = D3D12_DSV_FLAG_NONE;
        depth_view_description.Texture2D.MipSlice = 0;

        // create the depth/stencil view into the texture
        device->CreateDepthStencilView(resource_.Get(), &depth_view_description, depth_resource_view_.cpu_handle());

        resource_view_ =
            D3D12DescriptorManager::cpu_allocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).allocate_static();

        D3D12_SHADER_RESOURCE_VIEW_DESC shader_view_description = {0};
        shader_view_description.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        shader_view_description.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        shader_view_description.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        shader_view_description.Texture2D.MipLevels = 1;
        shader_view_description.Texture2D.MostDetailedMip = 0;

        device->CreateShaderResourceView(resource_.Get(), &shader_view_description, resource_view_.cpu_handle());
    }
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
