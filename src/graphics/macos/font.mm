#include "graphics/font.h"

#include <cmath>
#include <memory>
#include <vector>

#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreText/CoreText.h>

#include "core/exception.h"
#include "graphics/material.h"
#include "graphics/mesh_factory.h"
#include "graphics/sprite.h"
#include "graphics/texture.h"
#include "log/log.h"
#include "platform/macos/cf_ptr.h"
#include "platform/macos/macos_ios_utility.h"

namespace iris
{

/**
 * Struct containing implementation specific data.
 */
struct Font::implementation
{
    implementation()
        : font(nullptr),
          colour_space(nullptr),
          colour(nullptr),
          keys(),
          values(),
          attributes(nullptr)
    { }

    /** Font object. */
    CfPtr<CTFontRef> font;

    /** Colour space object. */
    CfPtr<CGColorSpaceRef> colour_space;

    /** Font colour. */
    CfPtr<CGColorRef> colour;

    /** Sting attribute keys. */
    std::array<CFStringRef, 2> keys;

    /** String attribute values. */
    std::array<CFTypeRef, 2> values;

    /** String attribute dictionary. */
    CfPtr<CFDictionaryRef> attributes;
};

Font::Font(
    const std::string &font_name,
    const std::uint32_t size,
    const std::string &text,
    const Vector3 &colour)
    : Sprite(0.0f, 0.0f, 1.0f, 1.0f, colour),
      font_name_(font_name),
      colour_(colour),
      impl_(std::make_unique<implementation>())
{
    // create a CoreFoundation string object from supplied Font name
    const auto font_name_cf = CfPtr<CFStringRef>(::CFStringCreateWithCString(
        kCFAllocatorDefault,
        font_name_.c_str(),
        kCFStringEncodingASCII));

    if(!font_name_cf)
    {
        throw Exception("failed to create CF string");
    }

    // create Font object
    impl_->font = CfPtr<CTFontRef>(::CTFontCreateWithName(
        font_name_cf.get(),
        size,
        nullptr));

    if(!impl_->font)
    {
        throw Exception("failed to create font");
    }

    // create a device dependant colour space
    impl_->colour_space = CfPtr<CGColorSpaceRef>(::CGColorSpaceCreateDeviceRGB());

    if(!impl_->colour_space)
    {
        throw Exception("failed to create colour space");
    }

    // create a CoreFoundation colour object from supplied colour
    const CGFloat components[] = { colour_.x, colour_.y, colour_.z, 1.0f };
    impl_->colour = CfPtr<CGColorRef>(::CGColorCreate(
        impl_->colour_space.get(),
        components));

    if(!impl_->colour)
    {
        throw Exception("failed to create colour");
    }

    impl_->keys = {{ kCTFontAttributeName, kCTForegroundColorAttributeName }};
    impl_->values = {{ impl_->font.get(), impl_->colour.get() }};

    // create string attributes dictionary, containing Font name and colour
    impl_->attributes = CfPtr<CFDictionaryRef>(::CFDictionaryCreate(
        kCFAllocatorDefault,
        reinterpret_cast<const void**>(impl_->keys.data()),
        reinterpret_cast<const void**>(impl_->values.data()),
        impl_->keys.size(),
        &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks));

    if(!impl_->attributes)
    {
        throw Exception("failed to create attributes");
    }

    LOG_DEBUG("font", "creating sprites for string: {}", text);

    // create CoreFoundation string object from supplied text
    const auto text_cf = CfPtr<CFStringRef>(::CFStringCreateWithCString(
        kCFAllocatorDefault,
        text.c_str(),
        kCFStringEncodingASCII));

    // create a CoreFoundation attributed string object
    const auto attr_string = CfPtr<CFAttributedStringRef>(::CFAttributedStringCreate(
        kCFAllocatorDefault,
        text_cf.get(),
        impl_->attributes.get()));

    const auto frame_setter = CfPtr<CTFramesetterRef>(
        ::CTFramesetterCreateWithAttributedString(attr_string.get()));

    // calculate minimal size required to render text
    CFRange range;
    const auto rect = ::CTFramesetterSuggestFrameSizeWithConstraints(
        frame_setter.get(),
        CFRangeMake(0, 0),
        nullptr,
        CGSizeMake(CGFLOAT_MAX, CGFLOAT_MAX),
        &range);

    // create a path object to render text
    const auto path = CfPtr<CGPathRef>(::CGPathCreateWithRect(
        CGRectMake(0, 0, std::ceil(rect.width), std::ceil(rect.height)),
        nullptr));

    if(!path)
    {
        throw Exception("failed to create path");
    }

    // create a frame to render text
    const auto frame = CfPtr<CTFrameRef>(::CTFramesetterCreateFrame(
        frame_setter.get(),
        range,
        path.get(),
        nullptr));

    if(!frame)
    {
        throw Exception("failed to create frame");
    }

    // round suggested width and height down to integers, multiply by two for
    // retina displays
    const auto width = static_cast<std::uint32_t>(rect.width) * 2u;
    const auto height = static_cast<std::uint32_t>(rect.height) * 2u;

    // allocate enough space to store RGBA tuples for each pixel
    std::vector<std::uint8_t> pixel_data(width * height * 4);

    const auto bits_per_pixel = 8u;
    const auto bytes_per_row = width * 4u;

    // create a context for rendering text
    const auto context = CfPtr<CGContextRef>(::CGBitmapContextCreateWithData(
        pixel_data.data(),
        width,
        height,
        bits_per_pixel,
        bytes_per_row,
        impl_->colour_space.get(),
        kCGImageAlphaPremultipliedLast,
        nullptr,
        nullptr));

    if(!context)
    {
        throw Exception("failed to create context");
    }

    const auto scale = platform::utility::screen_scale();
    LOG_ENGINE_DEBUG("font", "{}", scale);

    // ensure letters are rotated correct way and scaled for screen
    ::CGContextTranslateCTM(context.get(), 0.0f, rect.height * scale);
    ::CGContextScaleCTM(context.get(), scale, -scale);

    // render text, pixel data will be stored in our pixel data buffer
    ::CTFrameDraw(frame.get(), context.get());
    ::CGContextFlush(context.get());

    // create a Texture from the rendered pixel data
    Texture tex{ pixel_data, width, height, 4u };

    set_scale({ width, height, 1.0f });
    set_texture(std::move(tex));
}

Font::~Font() = default;

}

