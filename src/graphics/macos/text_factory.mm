////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/text_factory.h"

#include <array>
#include <cmath>
#include <memory>
#include <vector>

//#import <AppKit/AppKit.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreText/CoreText.h>

#include "core/auto_release.h"
#include "core/colour.h"
#include "core/data_buffer.h"
#include "core/error_handling.h"
#include "core/root.h"
#include "graphics/texture.h"
#include "graphics/texture_manager.h"
#include "log/log.h"

namespace iris::text_factory
{

Texture *create(const std::string &font_name, const std::uint32_t size, const std::string &text, const Colour &colour)
{
    // create a CoreFoundation string object from supplied Font name
    const auto font_name_cf = AutoRelease<CFStringRef, nullptr>{
        ::CFStringCreateWithCString(kCFAllocatorDefault, font_name.c_str(), kCFStringEncodingASCII), ::CFRelease};

    expect(font_name_cf, "failed to create CF string");

    // create Font object
    AutoRelease<CTFontRef, nullptr> font = {
        ::CTFontCreateWithName(font_name_cf.get(), static_cast<CGFloat>(size), nullptr), ::CFRelease};

    ensure(font, "failed to create font");

    // create a device dependant colour space
    AutoRelease<CGColorSpaceRef, nullptr> colour_space = {::CGColorSpaceCreateDeviceRGB(), ::CGColorSpaceRelease};

    expect(colour_space, "failed to create colour space");

    // create a CoreFoundation colour object from supplied colour
    const CGFloat components[] = {colour.r, colour.g, colour.b, colour.a};
    AutoRelease<CGColorRef, nullptr> font_colour = {::CGColorCreate(colour_space.get(), components), ::CGColorRelease};

    expect(font_colour, "failed to create colour");

    std::array<CFStringRef, 2> keys = {{kCTFontAttributeName, kCTForegroundColorAttributeName}};
    std::array<CFTypeRef, 2> values = {{font.get(), font_colour.get()}};

    // create string attributes dictionary, containing Font name and colour
    AutoRelease<CFDictionaryRef, nullptr> attributes = {
        ::CFDictionaryCreate(
            kCFAllocatorDefault,
            reinterpret_cast<const void **>(keys.data()),
            reinterpret_cast<const void **>(values.data()),
            keys.size(),
            &kCFTypeDictionaryKeyCallBacks,
            &kCFTypeDictionaryValueCallBacks),
        ::CFRelease};

    expect(attributes, "failed to create attributes");

    LOG_DEBUG("font", "creating sprites for string: {}", text);

    // create CoreFoundation string object from supplied text
    const auto text_cf = AutoRelease<CFStringRef, nullptr>{
        ::CFStringCreateWithCString(kCFAllocatorDefault, text.c_str(), kCFStringEncodingASCII), ::CFRelease};

    // create a CoreFoundation attributed string object
    const auto attr_string = AutoRelease<CFAttributedStringRef, nullptr>{
        ::CFAttributedStringCreate(kCFAllocatorDefault, text_cf.get(), attributes.get()), ::CFRelease};

    const auto frame_setter = AutoRelease<CTFramesetterRef, nullptr>{
        ::CTFramesetterCreateWithAttributedString(attr_string.get()), ::CFRelease};

    // calculate minimal size required to render text
    CFRange range;
    const auto rect = ::CTFramesetterSuggestFrameSizeWithConstraints(
        frame_setter.get(), CFRangeMake(0, 0), nullptr, CGSizeMake(CGFLOAT_MAX, CGFLOAT_MAX), &range);

    // create a path object to render text
    const auto path = AutoRelease<CGPathRef, nullptr>{
        ::CGPathCreateWithRect(CGRectMake(0, 0, std::ceil(rect.width), std::ceil(rect.height)), nullptr), nullptr};

    expect(path, "failed to create path");

    // create a frame to render text
    const auto frame = AutoRelease<CTFrameRef, nullptr>{
        ::CTFramesetterCreateFrame(frame_setter.get(), range, path.get(), nullptr), ::CFRelease};

    expect(frame, "failed to create frame");

    const auto scale = 2u;

    const auto width = static_cast<std::uint32_t>(rect.width) * scale;
    const auto height = static_cast<std::uint32_t>(rect.height) * scale;

    // allocate enough space to store RGBA tuples for each pixel
    DataBuffer pixel_data(width * height * 4);

    const auto bits_per_pixel = 8u;
    const auto bytes_per_row = width * 4u;

    // create a context for rendering text
    const auto context = AutoRelease<CGContextRef, nullptr>{
        ::CGBitmapContextCreateWithData(
            pixel_data.data(),
            width,
            height,
            bits_per_pixel,
            bytes_per_row,
            colour_space,
            kCGImageAlphaPremultipliedLast,
            nullptr,
            nullptr),
        nullptr};

    expect(context, "failed to create context");

    // render text, pixel data will be stored in our pixel data buffer
    ::CTFrameDraw(frame.get(), context.get());
    ::CGContextFlush(context.get());

    // create a Texture from the rendered pixel data
    auto *texture = Root::texture_manager().create(pixel_data, width, height, TextureUsage::IMAGE);

    return texture;
}

}
