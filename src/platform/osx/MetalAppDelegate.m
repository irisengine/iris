#import "osx/AppDelegate.h"

#import <Appkit/Appkit.h>
#import <CoreGraphics/CoreGraphics.h>
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>

#include "osx/MetalView.h"

@implementation AppDelegate

- (id)initWithRect:(NSRect)rect
{
    // call the super init, perform custom initialisation if this succeeds
    if(self = [super init])
    {
        [self setWidth:rect.size.width];
        [self setHeight:rect.size.height];

        MTLCreateSystemDefaultDevice();

        // create our window. It should have a title and render all content
        // to a buffer before being flushed, do not defer the creation of the
        // window
        NSWindow *window = [[NSWindow alloc]
            initWithContentRect:rect
            styleMask:NSWindowStyleMaskTitled
            backing:NSBackingStoreBuffered
            defer:NO];

        // create and setup a metal view
        MetalView* view = [[MetalView alloc] initWithFrame:rect device:MTLCreateSystemDefaultDevice()];
        [view setClearColor:MTLClearColorMake(0, 0, 0, 1)];
        [view setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
        [view setDepthStencilPixelFormat:MTLPixelFormatDepth32Float];

        // add the view to the window
        [window setContentView:view];

        // centre the window on the screen
        [window center];

        // display the view
        [window setAutodisplay:YES];

        // release window when it is closed
        [window setReleasedWhenClosed:YES];

        // show the window
        [window makeKeyAndOrderFront:self];

        [window makeFirstResponder:view];

        // redraw the view before displaying
        [view setNeedsDisplay:YES];

        // create and setup a metal layer
        CAMetalLayer *layer = [[CAMetalLayer alloc] init];
        [layer setDevice:[view device]];
        [layer setPixelFormat:MTLPixelFormatBGRA8Unorm];
        [layer setFrame:[view bounds]];
        [view.layer addSublayer:layer];

        // create a tracking area the size of the screen
        NSTrackingArea *tracking = [[NSTrackingArea alloc]
            initWithRect:rect
            options:NSTrackingMouseMoved|NSTrackingActiveAlways
            owner:view
            userInfo:nil];

        // add the tracking area
        [view addTrackingArea:tracking];

        // hide the cursor
        CGDisplayHideCursor(kCGDirectMainDisplay);
    }

    return self;
}

@end

