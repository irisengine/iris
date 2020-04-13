#include "font.hpp"

#include <cmath>
#include <memory>
#include <vector>

#import <AppKit/AppKit.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreText/CoreText.h>

#include "log.hpp"
#include "exception.hpp"
#include "material.hpp"
#include "osx/cf_ptr.hpp"
#include "shape_factory.hpp"
#include "sprite.hpp"
#include "texture.hpp"

namespace eng
{

/**
 * Struct containing implementation specific data.
 */
struct font::implementation
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
    cf_ptr<CTFontRef> font;

    /** Colour space object. */
    cf_ptr<CGColorSpaceRef> colour_space;

    /** Font colour. */
    cf_ptr<CGColorRef> colour;

    /** Sting attribute keys. */
    std::array<CFStringRef, 2> keys;

    /** String attribute values. */
    std::array<CFTypeRef, 2> values;

    /** String attribute dictionary. */
    cf_ptr<CFDictionaryRef> attributes;
};

font::font(
    const std::string &font_name,
    const std::uint32_t size,
    const vector3 &colour)
    : font_name_(font_name),
      colour_(colour),
      impl_(std::make_unique<implementation>())
{
    // create a CoreFoundation string object from supplied font name
    const auto font_name_cf = cf_ptr<CFStringRef>(::CFStringCreateWithCString(
        kCFAllocatorDefault,
        font_name_.c_str(),
        kCFStringEncodingASCII));

    if(!font_name_cf)
    {
        throw exception("failed to create CF string");
    }

    // create font object
    impl_->font = cf_ptr<CTFontRef>(::CTFontCreateWithName(
        font_name_cf.get(),
        size,
        nullptr));

    if(!impl_->font)
    {
        throw exception("failed to create font");
    }

    // create a device dependant colour space
    impl_->colour_space = cf_ptr<CGColorSpaceRef>(::CGColorSpaceCreateDeviceRGB());

    if(!impl_->colour_space)
    {
        throw exception("failed to create colour space");
    }

    // create a CoreFoundation colour object from supplied colour
    const CGFloat components[] = { colour_.x, colour_.y, colour_.z, 1.0f };
    impl_->colour = cf_ptr<CGColorRef>(::CGColorCreate(
        impl_->colour_space.get(),
        components));

    if(!impl_->colour)
    {
        throw exception("failed to create colour");
    }

    impl_->keys = {{ kCTFontAttributeName, kCTForegroundColorAttributeName }};
    impl_->values = {{ impl_->font.get(), impl_->colour.get() }};

    // create string attributes dictionary, containing font name and colour
    impl_->attributes = cf_ptr<CFDictionaryRef>(::CFDictionaryCreate(
        kCFAllocatorDefault,
        reinterpret_cast<const void**>(impl_->keys.data()),
        reinterpret_cast<const void**>(impl_->values.data()),
        impl_->keys.size(),
        &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks));

    if(!impl_->attributes)
    {
        throw exception("failed to create attributes");
    }
}

font::~font() = default;
font::font(font&&) = default;
font& font::operator=(font&&) = default;

std::shared_ptr<sprite> font::sprites(
    const std::string &text,
    const float x,
    const float y)
{
    LOG_DEBUG("font", "creating sprites for string: {}", text);

    // create CoreFoundation string object from supplied text
    const auto text_cf = cf_ptr<CFStringRef>(::CFStringCreateWithCString(
        kCFAllocatorDefault,
        text.c_str(),
        kCFStringEncodingASCII));

    // create a CoreFoundation attributed string object
    const auto attr_string = cf_ptr<CFAttributedStringRef>(::CFAttributedStringCreate(
        kCFAllocatorDefault,
        text_cf.get(),
        impl_->attributes.get()));

    const auto frame_setter = cf_ptr<CTFramesetterRef>(
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
    const auto path = cf_ptr<CGPathRef>(::CGPathCreateWithRect(
        CGRectMake(0, 0, std::ceil(rect.width), std::ceil(rect.height)),
        nullptr));

    if(!path)
    {
        throw exception("failed to create path");
    }

    // create a frame to render text
    const auto frame = cf_ptr<CTFrameRef>(::CTFramesetterCreateFrame(
        frame_setter.get(),
        range,
        path.get(),
        nullptr));

    if(!frame)
    {
        throw exception("failed to create frame");
    }

    // round suggested width and height down to integers, multiply by two for
    // retina displays
    const auto width = static_cast<std::uint32_t>(rect.width) * 2u;
    const auto height = static_cast<std::uint32_t>(rect.height) * 2u;

    LOG_DEBUG("font", "w {} h {}", rect.width, rect.height);
    LOG_DEBUG("font", "w {} h {}", width, height);

    // allocate enough space to store RGBA tuples for each pixel
    std::vector<std::uint8_t> pixel_data(width * height * 4);

    const auto bits_per_pixel = 8u;
    const auto bytes_per_row = width * 4u;

    // create a context for rendering text
    const auto context = cf_ptr<CGContextRef>(::CGBitmapContextCreateWithData(
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
        throw exception("failed to create context");
    }

    auto *window = [[NSApp windows] firstObject];
    const auto scale = [[window screen] backingScaleFactor];
    const auto window_frame = [window frame];

    // ensure letters are rotated correct way and scaled for screen
    ::CGContextTranslateCTM(context.get(), 0.0f, rect.height * scale);
    ::CGContextScaleCTM(context.get(), scale, -scale);

    // render text, pixel data will be stored in our pixel data buffer
    ::CTFrameDraw(frame.get(), context.get());
    ::CGContextFlush(context.get());

    // create a texture from the rendered pixel data
    texture tex{ pixel_data, width, height, 4u };

    // create a sprite to render the texture
    return std::make_shared<sprite>(shape_factory::sprite(
        x,
        y,
        (width / window_frame.size.width),
        (height / window_frame.size.height),
        colour_,
        std::move(tex)));
}

}

