#include "graphics/text_factory.h"

#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <windows.h>

#include "core/auto_release.h"
#include "core/exception.h"
#include "core/root.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"
#include "log/log.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite")

namespace
{

/**
 * Helper function to convert a utf-8 string to a utf-16 string.
 *
 * @param str
 *   String to covert.
 *
 * @returns
 *   Converted string.
 */
std::wstring widen(const std::string &str)
{
    // calculate number of characters in wide string
    const auto expected_size = ::MultiByteToWideChar(
        CP_UTF8,
        MB_PRECOMPOSED,
        str.c_str(),
        static_cast<int>(str.length()),
        NULL,
        0);

    if (expected_size == 0)
    {
        throw iris::Exception("could not get wstring size");
    }

    std::wstring wide_str(expected_size, L'\0');

    // widen
    if (::MultiByteToWideChar(
            CP_UTF8,
            MB_PRECOMPOSED,
            str.c_str(),
            static_cast<int>(str.length()),
            wide_str.data(),
            static_cast<int>(wide_str.length())) != expected_size)
    {
        throw iris::Exception("could not widen string");
    }

    return wide_str;
}

/**
 * Helper function to call Release on an object.
 *
 * @param ptr
 *   Ptr to release.
 */
template <class T>
void SafeRelease(T ptr)
{
    if (ptr != nullptr)
    {
        ptr->Release();
    }
}

}

namespace iris::text_factory
{

Texture *create(
    const std::string &font_name,
    const std::uint32_t size,
    const std::string &text,
    const Colour &colour)
{
    // create factory for creating Direct2d objects
    ID2D1Factory *direct2d_factory = nullptr;
    if (::D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED, &direct2d_factory) != S_OK)
    {
        throw Exception("failed to create d2d factory");
    }

    // create factory for creating DirectWrite objects
    IDWriteFactory *write_factory = nullptr;
    if (::DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown **>(&write_factory)) != S_OK)
    {
        throw Exception("failed to create write factory");
    }

    // widen font name for win32 api calls
    const auto font_name_wide = widen(font_name);

    // create object describing text format
    IDWriteTextFormat *text_format = nullptr;
    if (write_factory->CreateTextFormat(
            font_name_wide.c_str(),
            NULL,
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            static_cast<float>(size),
            L"en-us",
            &text_format) != S_OK)
    {
        throw Exception("failed to create text format");
    }

    // widen text for win32 api calls
    const auto text_wide = widen(text);

    // create object for text layout
    // we assume a very large size, but can query after this call for actual
    // size
    IDWriteTextLayout *text_layout = nullptr;
    if (write_factory->CreateTextLayout(
            text_wide.c_str(),
            static_cast<UINT32>(text_wide.length()),
            text_format,
            99999.0f,
            99999.0f,
            &text_layout) != S_OK)
    {
        throw Exception("failed to create text layout");
    }

    // get actual text dimension
    DWRITE_TEXT_METRICS metrics = {0};
    text_layout->GetMetrics(&metrics);

    const auto width = static_cast<std::uint32_t>(metrics.width);
    const auto height = static_cast<std::uint32_t>(metrics.height);

    // create factory for creating WIC objects
    IWICImagingFactory *image_factory = nullptr;
    if (::CoCreateInstance(
            CLSID_WICImagingFactory2,
            NULL,
            CLSCTX_INPROC_SERVER,
            __uuidof(IWICImagingFactory2),
            reinterpret_cast<LPVOID *>(&image_factory)) != S_OK)
    {
        throw Exception("failed to create image factory");
    }

    // create a bitmap to render text to
    IWICBitmap *bitmap = nullptr;
    if (image_factory->CreateBitmap(
            width,
            height,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapCacheOnDemand,
            &bitmap) != S_OK)
    {
        throw Exception("failed to create bitmap");
    }

    // default render properties
    const auto properties = ::D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        ::D2D1::PixelFormat(
            DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0.f,
        0.f,
        D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);

    // create a render target to render font to bitmap
    AutoRelease<ID2D1RenderTarget *, nullptr> render_target = {
        nullptr, &SafeRelease<ID2D1RenderTarget *>};
    if (direct2d_factory->CreateWicBitmapRenderTarget(
            bitmap, &properties, &render_target) != S_OK)
    {
        throw Exception("failed to create render target");
    }

    // create a brush with supplied colour
    AutoRelease<ID2D1SolidColorBrush *, nullptr> brush = {
        nullptr, &SafeRelease<ID2D1SolidColorBrush *>};
    if (render_target.get()->CreateSolidColorBrush(
            ::D2D1::ColorF(
                ::D2D1::ColorF::ColorF(colour.r, colour.g, colour.b)),
            &brush) != S_OK)
    {
        throw Exception("failed to create brush");
    }

    const auto origin = ::D2D1::Point2F(0.0f, 0.0f);

    // render text
    render_target.get()->BeginDraw();
    render_target.get()->DrawTextLayout(origin, text_layout, brush);
    render_target.get()->EndDraw();

    // get size of bitmap
    UINT bitmap_width = 0u;
    UINT bitmap_height = 0u;
    if (bitmap->GetSize(&bitmap_width, &bitmap_height) != S_OK)
    {
        throw Exception("failed to get bitmap size");
    }

    WICRect rect = {0, 0, (INT)bitmap_width, (INT)bitmap_height};

    // lock bitmap to read data
    // will auto release lock at end of function scope
    AutoRelease<IWICBitmapLock *, nullptr> lock = {
        nullptr, [](IWICBitmapLock *lock) { lock->Release(); }};
    if (bitmap->Lock(&rect, WICBitmapLockRead, &lock) != S_OK)
    {
        throw Exception("failed to get lock");
    }

    // get pointer to raw bitmap data
    UINT buffer_size = 0u;
    std::byte *buffer = nullptr;
    if (lock.get()->GetDataPointer(
            &buffer_size, reinterpret_cast<BYTE **>(&buffer)) != S_OK)
    {
        throw Exception("failed to get data pointer");
    }

    DataBuffer pixel_data(buffer, buffer + buffer_size);

    // we have rendered font as BGRA, so swap red and blue
    for (auto i = 0u; i < pixel_data.size(); i += 4u)
    {
        std::swap(pixel_data[i], pixel_data[i + 2u]);
    }

    auto *texture = Root::texture_manager().load(
        pixel_data, width, height, PixelFormat::RGBA);
    texture->set_flip(true);

    return texture;
}
}