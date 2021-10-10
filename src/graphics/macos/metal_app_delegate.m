////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#import "graphics/macos/metal_app_delegate.h"

#import <Appkit/Appkit.h>
#import <CoreGraphics/CoreGraphics.h>
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>

#include "graphics/macos/metal_view.h"

@implementation MetalAppDelegate

- (id)initWithRect:(NSRect)rect
{
    // call the super init, perform custom initialisation if this succeeds
    if (self = [super init])
    {
        [self setWidth:rect.size.width];
        [self setHeight:rect.size.height];

        MTLCreateSystemDefaultDevice();

        // create our window. It should have a title and render all content
        // to a buffer before being flushed, do not defer the creation of the
        // window
        NSWindow *window = [[NSWindow alloc] initWithContentRect:rect
                                                       styleMask:NSWindowStyleMaskTitled
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];

        // create and setup a metal view
        MetalView *view = [[MetalView alloc] initWithFrame:rect device:MTLCreateSystemDefaultDevice()];
        [view setClearColor:MTLClearColorMake(0, 0, 0, 1)];
        [view setColorPixelFormat:MTLPixelFormatRGBA16Float];
        [view setDepthStencilPixelFormat:MTLPixelFormatDepth32Float];

        CAMetalLayer *layer = (CAMetalLayer *)view.layer;

        const CFStringRef name = kCGColorSpaceExtendedSRGB;
        CGColorSpaceRef colorspace = CGColorSpaceCreateWithName(name);
        layer.colorspace = colorspace;
        layer.wantsExtendedDynamicRangeContent = YES;

        // add the view to the window
        [window setContentView:view];

        // centre the window on the screen
        [window center];

        // release window when it is closed
        [window setReleasedWhenClosed:YES];

        // show the window
        [window makeKeyAndOrderFront:self];

        [window makeFirstResponder:view];

        [window setColorSpace:[NSColorSpace genericRGBColorSpace]];

        // redraw the view before displaying
        [view setNeedsDisplay:YES];

        // create a tracking area the size of the screen
        NSTrackingArea *tracking = [[NSTrackingArea alloc] initWithRect:rect
                                                                options:NSTrackingMouseMoved | NSTrackingActiveAlways
                                                                  owner:view
                                                               userInfo:nil];

        // add the tracking area
        [view addTrackingArea:tracking];

        // hide and lock the cursor
        CGDisplayHideCursor(kCGDirectMainDisplay);
        CGAssociateMouseAndMouseCursorPosition(NO);
    }

    return self;
}

@end
