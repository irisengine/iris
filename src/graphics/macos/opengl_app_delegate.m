////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#import "graphics/macos/opengl_app_delegate.h"

#import <Appkit/Appkit.h>
#import <CoreGraphics/CoreGraphics.h>
#import <Foundation/Foundation.h>

#import "graphics/macos/opengl_view.h"

@implementation OpenGLAppDelegate

- (id)initWithRect:(NSRect)rect
{
    // call the super init, perform custom initialisation if this succeeds
    if (self = [super init])
    {
        [self setWidth:rect.size.width];
        [self setHeight:rect.size.height];

        // create our window. It should have a title and render all content
        // to a buffer before being flushed, do not defer the creation of the
        // window
        NSWindow *window =
            [[NSWindow alloc] initWithContentRect:rect
                                        styleMask:NSWindowStyleMaskTitled
                                          backing:NSBackingStoreBuffered
                                            defer:NO];

        // here we specify the attributes of the OpenGl view.
        NSOpenGLPixelFormatAttribute pixelFormatAttributes[] = {
            NSOpenGLPFAOpenGLProfile,
            NSOpenGLProfileVersion3_2Core, // use at least OpenGl 3.2
            NSOpenGLPFAColorSize,
            32, // 32 bit colour
            NSOpenGLPFAAlphaSize,
            8, // 8 bit alpha
            NSOpenGLPFADepthSize,
            32,                      // 32 bit depth buffer
            NSOpenGLPFADoubleBuffer, // use double buffering
            NSOpenGLPFAAccelerated,  // use hardware acceleration
            0                        // array termination
        };

        // create the pixel format object with the above attributes
        NSOpenGLPixelFormat *pixel_format = [[NSOpenGLPixelFormat alloc]
            initWithAttributes:pixelFormatAttributes];

        // create our OpenGl view, make it the same size as the window
        OpenGLView *view = [[OpenGLView alloc] initWithFrame:rect
                                                 pixelFormat:pixel_format];

        // ensure OpenGL fully utilises retina displays
        [view setWantsBestResolutionOpenGLSurface:YES];

        // add the view to the window
        [window setContentView:view];

        // centre the window on the screen
        [window center];

        // release window when it is closed
        [window setReleasedWhenClosed:YES];

        // show the window
        [window makeKeyAndOrderFront:self];

        // make this the current OpenGl context
        [[view openGLContext] makeCurrentContext];

        // redraw the view before displaying
        [view setNeedsDisplay:YES];

        [window makeFirstResponder:view];

        // create a tracking area the size of the screen
        NSTrackingArea *tracking = [[NSTrackingArea alloc]
            initWithRect:rect
                 options:NSTrackingMouseMoved | NSTrackingActiveAlways
                   owner:view
                userInfo:nil];

        // add the tracking area
        [view addTrackingArea:tracking];

        // hide the cursor
        CGDisplayHideCursor(kCGDirectMainDisplay);
        CGAssociateMouseAndMouseCursorPosition(NO);

        // disable vsync
        int value = 0;
        CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &value);
    }

    return self;
}

@end
